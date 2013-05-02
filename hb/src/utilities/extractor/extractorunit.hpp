#ifndef extractorunit_h__
#define extractorunit_h__

#include <set>
#include <map>
#include "csvparser.hpp"
#include "hashergenerator.hpp"
#include "rawdatareader.hpp"

struct HeaderDesc{
	std::string classname;
	std::map<std::string, int> colnameindmap;
	std::vector<std::string> colnames;
	std::vector<std::string> coltypes;
	std::vector<int> datasizes;
	std::vector<int> dsdiffidx;
	std::vector<std::string> comments;
	bool initialized;

	HeaderDesc() : initialized(false) {}

	inline unsigned int size(){
		return datasizes.size();
	}

	inline int name2ind(const std::string& colname) const {
		if (colnameindmap.find(colname) == colnameindmap.end())
			return -1;
		else
			return colnameindmap.find(colname)->second;
	}
	
	inline void clear(){
		initialized = false; colnames.clear(); coltypes.clear(); datasizes.clear(); colnameindmap.clear();
		dsdiffidx.clear();
	}
	inline void push(const std::string& __colname, const std::string& __coltype, int __datasize, const std::string& __comment = ""){
		initialized = true;
		colnames.push_back(__colname);
		coltypes.push_back(__coltype);
		if (datasizes.size()==0){
			dsdiffidx.push_back(0); }
		else{
			dsdiffidx.push_back(dsdiffidx.back() + datasizes.back()); }
		datasizes.push_back(__datasize);
		comments.push_back(__comment);
		colnameindmap[colnames.back()] = colnames.size()-1;
	}
};

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
	std::map<std::string, std::string> code2header;

	inline void parse_srcfile(){
		code2header.clear();
		headercodes.clear();
		headernames.clear();
		HeaderDesc hd;
		hds.clear();
		hdname2idx.clear();
		CsvParser csvin(srcfile);
		while(csvin.getline()){
			int n = -1; std::string colname, coltype; int datasize = 0;
			std::string comment;
			csvin >> n >> colname >> coltype >> datasize;
			csvin.gets(comment);
			if (n == -1) continue;
			if (n == 0){
				headernames.push_back(colname);
				headercodes.push_back(comment);
				code2header[headercodes.back()] = headernames.back();
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

	std::vector<std::vector<std::string> > ref_cols;
	std::map <std::string, int> ref_header2ind;

	inline void input_datafiles_output_reformatfile(){
		ref_cols.clear();
		ref_header2ind.clear();
		{
			CsvParser descin(wanteddescfile);
			while(descin.getline()){
				std::vector<std::string> col;
				while(!descin.empty()){
					col.push_back(descin.getstr());
				}
				ref_header2ind[col[0]] = ref_cols.size();
				ref_cols.push_back(col);
			}
		}

		FILE *fo;
		fopen_s(&fo,outputfile.c_str(),"wt");
		for (int i=1;i<(int)ref_cols[0].size();++i){
			if (i>1)
				fprintf(fo,",");
			fprintf(fo,"%s",ref_cols[0][i].c_str());
		}
		fprintf(fo,"\n");

		int counter=0;
		for (int dfi=0;dfi<(int)datafiles.size();++dfi){
			RawDataReader rd(datafiles[dfi]);

			while(rd.next()){
				char* msgbuf = rd.msgbuf;
				std::string key(msgbuf,msgbuf+5);

				if(code2header.find(key)==code2header.end()){
					printf("%d %s %s\n",rd.sz,key.c_str(),msgbuf);
				}
				std::string headername=code2header[key];
				if(ref_header2ind.find(headername)==ref_header2ind.end()) continue;
				if (hdname2idx.find(headername)==hdname2idx.end()) continue;
				int refidx = ref_header2ind.find(headername)->second;
				int realidx = hdname2idx.find(headername)->second;

				for (int i=1;i<(int)ref_cols[refidx].size();++i){
					std::string fmt = ref_cols[refidx][i];
					if (fmt.length()==0); // shown as empty column.
					else if (fmt[0]=='$'){ // special input.
						if (fmt[1]=='+'){
							fprintf(fo,"%d",++counter);
						}
						else if (fmt[1]=='_'){
							fprintf(fo,"%s",fmt.substr(2).c_str());
						}
						else if ((fmt.substr(1).compare("TIME"))==0){
							fprintf(fo,"notime..sorry");
						}
					}
					else{
						const HeaderDesc& hdsc = hds[realidx];
						int idx = hdsc.name2ind(fmt);
						int stj = hdsc.dsdiffidx[idx];
						int lenj = hdsc.datasizes[idx];
						for (int j=stj; j<stj+lenj ;++j){
							fprintf(fo,"%c",msgbuf[j]);
						}
					}
					fprintf(fo,",");
				}
				fprintf(fo,"\n");
			}
		}
		fclose(fo);
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
