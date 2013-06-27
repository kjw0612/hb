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
		headergenerateorder,
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
		if (hd.initialized){
			hdname2idx[hd.classname] = hds.size();
			hds.push_back(hd);
		}
	}

	void output_objectfile_order(){
		std::ofstream fo(objectfile.c_str());
		std::string ofdef = objectfile;
		std::replace(ofdef.begin(),ofdef.end(),'.','_');
		std::transform(ofdef.begin(),ofdef.end(),ofdef.begin(),::toupper);
		fo << "#ifndef " << ofdef << std::endl;
		fo << "#define " << ofdef << std::endl;

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

	void output_objectfile(){
		std::ofstream fo(objectfile.c_str());
		std::string ofdef = objectfile;
		std::replace(ofdef.begin(),ofdef.end(),'.','_');
		std::transform(ofdef.begin(),ofdef.end(),ofdef.begin(),::toupper);
		fo << "#ifndef " << ofdef << std::endl;
		fo << "#define " << ofdef << std::endl;
		fo << std::endl;
		fo << "#include \"jhhash.hpp\"" << std::endl << std::endl;

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

	inline void generate_order_header(){
		parse_srcfile();
		output_objectfile_order();
	}

	inline void generate_header(){
		std::ofstream fo("jhhash.hpp");
		fo << "#ifndef jhhash_hpp" << std::endl
			<< "#define jhhash_hpp" << std::endl;
		fo << hashfunc::jhhash_str() << std::endl;
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
		std::map<std::string, std::string> tar_varmap;


		for (int dfi=0;dfi<(int)datafiles.size();++dfi){
			RawDataReader rd(datafiles[dfi]);
			long long rdtsc_st = 0, diff_rdtsc = 0, prev_rdtsc = 0, prev_timestamp = 0, first_timestamp = 0;
			long double filter_ratio = 0.95;
			long double accum_rdtsc = 0, accum_timestamp = 0;

			while(rd.next()){
				char* msgbuf = rd.msg;
				std::string key(msgbuf,msgbuf+5);

				if(code2header.find(key)==code2header.end()){
					printf("%d %s %s\n",rd.sz,key.c_str(),msgbuf);
				}
				std::string headername=code2header[key];
				if(ref_header2ind.find(headername)==ref_header2ind.end()) continue;
				if (hdname2idx.find(headername)==hdname2idx.end()) continue;
				int refidx = ref_header2ind.find(headername)->second;
				int realidx = hdname2idx.find(headername)->second;

				const HeaderDesc& hdsc = hds[realidx];
				int idx = hdsc.name2ind("krcode");
				int stj = hdsc.dsdiffidx[idx];
				int lenj;
				//lenj = hdsc.datasizes[idx];
				lenj = target_krcode.length();
				char krcodebuf[50] = "";
				for (int j=stj; j<stj+lenj ;++j){
					krcodebuf[j-stj] = msgbuf[j];
				}
				if (_strcmpi(target_krcode.c_str(),krcodebuf)){ // not same
					continue;
				}

				for (int i=1;i<(int)ref_cols[refidx].size();++i){
					std::string fmt = ref_cols[refidx][i];
					std::string target_var;
					int target_var_mode = 0, prtflag = 0;
					if (fmt.length()==0); // shown as empty column.
					else if (fmt[0]=='$'){ // special input.
						if (fmt[1]=='+'){
							fprintf(fo,"%d",++counter);
						}
						else if (fmt[1]=='_'){
							fprintf(fo,"%s",fmt.substr(2).c_str());
						}
						else if (fmt[1]=='>'){
							target_var = fmt.substr(2);
							target_var_mode = 2;
						}
						else if (fmt[1]=='<'){
							target_var = fmt.substr(2);
							target_var_mode = 1;
							prtflag = 1;
						}
						else if ((fmt.substr(1).compare("TIME"))==0){
							long long d_timestamp = atoi(tar_varmap["timestamp"].c_str());
							long long valPrint = 0;
							if (d_timestamp ==0){
								valPrint = 8000000;
							}
							else{
								if (rdtsc_st == 0){
									rdtsc_st = rd.rt;
									first_timestamp = d_timestamp;
									valPrint = first_timestamp;
								}
								else{
									diff_rdtsc = rd.rt - rdtsc_st;
									accum_rdtsc = (accum_rdtsc * filter_ratio) + (rd.rt - rdtsc_st) * (1-filter_ratio);
									accum_timestamp = accum_timestamp * filter_ratio + (d_timestamp - first_timestamp) * (1-filter_ratio);
									valPrint = first_timestamp + (long long)(diff_rdtsc * ((long double)accum_timestamp / accum_rdtsc));
								}
								prev_rdtsc = rdtsc_st;
								prev_timestamp = d_timestamp;
								//fprintf(fo,"notime..sorry");
							}
							fprintf(fo,"%s %lld",target_dates[dfi].c_str(),valPrint);
						}
					}
					else{ prtflag = 1; target_var = fmt; }

					if (prtflag){
						idx = hdsc.name2ind(target_var);
						stj = hdsc.dsdiffidx[idx];
						lenj = hdsc.datasizes[idx];
						char ending_buf[50] = "";
						for (int j=stj; j<stj+lenj ;++j){
							fprintf(fo,"%c",msgbuf[j]);
							ending_buf[j-stj] = msgbuf[j];
						}
						if (target_var_mode == 1){
							tar_varmap[target_var] = ending_buf;
						}
					}
					if (target_var_mode == 2){
						fprintf(fo,"%s",tar_varmap[target_var].c_str());
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
		else if (type == headergenerateorder){
			generate_order_header();
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
	std::string target_krcode;
	std::vector<std::string> target_dates;
};

#endif // extractorunit_h__
