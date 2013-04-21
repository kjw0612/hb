#ifndef extractorunit_h__
#define extractorunit_h__

#include <set>
#include <map>
#include "csvparser.hpp"
#include "hashergenerator.hpp"

struct HeaderDesc{
	std::string classname;
	std::vector<std::string> colnames;
	std::vector<std::string> coltypes;
	std::vector<int> datasizes;
	std::vector<std::string> comments;
	bool initialized;

	HeaderDesc() : initialized(false) {}
	inline unsigned int size(){
		return datasizes.size();
	}
	
	inline void clear(){
		initialized = false; colnames.clear(); coltypes.clear(); datasizes.clear();
	}
	inline void push(const std::string& __colname, const std::string& __coltype, int __datasize, const std::string& __comment = ""){
		initialized = true;
		colnames.push_back(__colname);
		coltypes.push_back(__coltype);
		datasizes.push_back(__datasize);
		comments.push_back(__comment);
	}
};

inline void fGetData(char *p, int len, FILE *fp ){
	while (len -->0 && !feof(fp) ){
		*p++ = fgetc(fp);
	}
}

class ExtractorUnit{
public:
	enum ExecutionType{
		headergenerate,
		reformat,
	};

	std::vector<HeaderDesc> hds;
	std::map<std::string, int> hdname2idx;
	std::vector<std::string> headercodes;
	std::vector<std::string> headernames;

	inline void parse_srcfile(){
		headercodes.clear();
		headernames.clear();
		HeaderDesc hd;
		hds.clear();
		hdname2idx.clear();
		CsvParser csvin(srcfile);
		while(csvin.getline()){
			int n=0; std::string colname, coltype; int datasize = 0;
			std::string comment;
			csvin >> n >> colname >> coltype >> datasize;
			csvin.gets(comment);
			if (n == 0){
				headernames.push_back(colname);
				headercodes.push_back(comment);
				if (hd.initialized){
					hdname2idx[hd.classname] = hds.size();
					hds.push_back(hd);
				}
				hd.clear();
				hd.classname = colname;
			}
			else{
				hd.push(colname, coltype, datasize, comment);
			}
		}
	}

	void output_objectfile(){
		std::ofstream fo(objectfile.c_str());
		std::string ofdef = objectfile;
		std::replace(ofdef.begin(),ofdef.end(),'.','_');
		std::transform(ofdef.begin(),ofdef.end(),ofdef.begin(),::toupper);
		fo << "#ifndef " << ofdef << std::endl;
		fo << "#define " << ofdef << std::endl;
		fo << std::endl;
		fo << "#include \"jshash.hpp\"" << std::endl << std::endl;

		fo << hashergenerator(headercodes,headernames,name) << std::endl;

		for (int i=0;i<(int)hds.size();++i){
			HeaderDesc& hd = hds[i];
			fo << std::endl;
			fo << "struct " << hd.classname << "{" << std::endl;
			for (int j=0;j<(int)hd.size();++j){
				fo << "\t";
				if (::tolower(hd.coltypes[j][0])=='9'){
					fo << "unsigned ";
				}
				fo << "char ";
				fo << hd.colnames[j] << "[" << hd.datasizes[j] << "];";
				if (hd.comments[j].length()>0){
					fo << "\t// " << hd.comments[j];
				}
				fo << std::endl;
			}
			fo << "};" << std::endl;
		}
		fo << "#endif";
		fo.close();
	}

	inline void generate_header(){
		std::ofstream fo("jshash.hpp");
		fo << "#ifndef jshash_hpp" << std::endl
			<< "#define jshash_hpp" << std::endl;
		fo << hashfunc::jshash_str() << std::endl;
		fo << "#endif";
		fo.close();
		parse_srcfile();
		output_objectfile();
	}

	std::map<std::string, std::string> type2code;
	std::map<std::string, std::string> code2type;
	std::set<std::string> showntypes;
	std::map<std::string,std::vector<std::string> > type2showncols;

	inline void input_datafiles_output_reformatfile(){
		char msgbuf[2001], buf[2001];
		{
			CsvParser csvin(typesfile);
			type2code.clear();
			code2type.clear();
			while(csvin.getline()){
				std::string headertype = "", code = "";
				csvin >> headertype >> code;
				if (code.length()==0){
					break; }
				type2code[headertype] = code;
				code2type[code] = headertype;
			}
		}
		long long rt;
		int sz;
		{
			FILE *fp;
			fopen_s(&fp,wanteddescfile.c_str(),"rt");
			int n=0,m=0;
			fscanf_s(fp,"%d",&n);
			for (int i=0;i<n;++i){
				fscanf_s(fp,"%s",buf, sizeof(buf));
				showntypes.insert(buf);
				fscanf_s(fp,"%d",&m);
				std::vector<std::string> showncols(m);
				for (int j=0;j<m;++j){
					fscanf_s(fp,"%s",buf, sizeof(buf));
					showncols[j] = buf;
				}
				type2showncols[buf] = showncols;
			}
			fclose(fp);
		}

		FILE *fo;
		fopen_s(&fo,outputfile.c_str(),"wt");
		for (int i=0;i<(int)datafiles.size();++i){
			FILE *fp;
			fopen_s(&fp,datafiles[i].c_str(),"rb");
			if (!fp)
				continue;

			while(!feof(fp)){
				fGetData((char *)&rt, 8, fp);
				fGetData((char *)&sz, 4, fp);
				memset(msgbuf,0,sizeof(msgbuf));
				fGetData(msgbuf, sz, fp);
				std::string key(msgbuf,msgbuf+5);
				if(code2type.find(key)==code2type.end()){
					printf("%d %s %s\n",sz,key.c_str(),msgbuf);
				}
				std::string name=code2type[key];
				if(showntypes.find(name)==showntypes.end()) continue;

				if (hdname2idx.find(name)!=hdname2idx.end()){
					fprintf(fo,"%lld,%s",rt,name.c_str());
					int idx = hdname2idx.find(name)->second;
					int sti = 0, leni = 0;
					for (int j=0;j<(int)hds[idx].size();++j){
						leni = hds[idx].datasizes[j];
						memcpy(buf,msgbuf+sti,leni);
						buf[leni] = 0;
						fprintf(fo,",%s",buf);
						sti += leni;
					}
					fprintf(fo,"\n");
				}
				/*
				vector<int> vct=structtolen[name];
				vector<string> vcts=structtoname[name];
				const char *namestr = name.c_str();
				fprintf(fo,"%lld,%s",rt,namestr);
				int cnt;
				for(l=0;l<wantcol.size();l++){
					cnt=0;
					for(j=0;j<vct.size();j++){
						if(wantcol[l] == vcts[j]){
							for(k=cnt;k<cnt+vct[j];k++){
								buf[k-cnt]=msgbuf[k];
							}
							buf[vct[j]]=0;
							fprintf(fo,",%s",buf);
							break;
						}
						cnt+=vct[j];
					}
				}*/
				/*			if(cnt!=sz){
				cnttt++;
				printf("%s %d %d\n",name.c_str(),cnt,sz);
				}*/
			}
			fclose(fp);
		}
	}

	inline void reformat_datafiles(){
		parse_srcfile();
		input_datafiles_output_reformatfile();
	}

	inline void proceed(){
		if (type == headergenerate){
			generate_header();
		}
		else{
			reformat_datafiles();
		}
	}

	ExecutionType type;
	std::string srcfile;
	std::string typesfile;
	// headergenerate
	std::string objectfile;
	// reformatter
	std::vector<std::string> datafiles;
	std::string outputfile;
	std::string wanteddescfile;
	std::string name;
};

#endif // extractorunit_h__
