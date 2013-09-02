#ifndef extractorunit_h__
#define extractorunit_h__

#include <set>
#include <map>
#include "csvparser.hpp"
#include "hashergenerator.hpp"
#include "rawdatareader.hpp"
#include "functional.h"
#include "date.hpp"

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
	std::vector<std::vector<std::string> > ref_cols;
	std::map <std::string, int> ref_header2ind;

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

	struct RdtscConvBase{
		long long rdtsc_st, diff_rdtsc, prev_rdtsc, prev_timestamp, first_timestamp, d_timestamp;
		long double filter_ratio, accum_rdtsc, accum_timestamp;
		RdtscConvBase(){
			rdtsc_st = 0, diff_rdtsc = 0, prev_rdtsc = 0, prev_timestamp = 0, first_timestamp = 0, d_timestamp = 0;
			filter_ratio = 0.95;
			accum_rdtsc = 0, accum_timestamp = 0;
		}
	};

	struct RfmPerCode{

		int counter;
		FILE *fo;
		RfmPerCode() : counter(0), fo(NULL), askQty1(0), bidQty1(0), askPrc1(0), bidPrc1(0), dir(0), priceChanged(0), price_var(0) {}

		RfmPerCode(const std::string& outputfolder, const std::string& krcode, const std::string& target_date, const std::vector<std::vector<std::string> >& ref_cols) : counter(0){
			std::string outputfile = outputfolder + krcode + "_" + target_date + ".csv";
			fopen_s(&fo,outputfile.c_str(),"wt");
			for (int i=1;i<(int)ref_cols[0].size();++i){
				if (i>1)
					fprintf(fo,",");
				fprintf(fo,"%s",ref_cols[0][i].c_str());
			}
			fprintf(fo,"\n");
		}

		void print(const std::vector<std::vector<std::string> >& ref_cols, int refidx,
			RawDataReader &rd, const char* msgbuf, const std::string& target_date, const HeaderDesc& hdsc, RdtscConvBase& rcb)
		{
			priceChanged = 0;
			for (int i=1;i<(int)ref_cols[refidx].size();++i){
				std::string fmt = ref_cols[refidx][i];
				std::string target_var;
				int target_var_mode = 0, prtflag = 0;
				if (ref_cols[0][i][0]==' '){
					fprintf(fo," ");
				}
				if (fmt.length()==0){ // shown as empty column.
					fprintf(fo," ");
				}
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
					else if (fmt.substr(1).compare("DIR")==0){
						fprintf(fo,"%d",dir);
					}
					else if ((fmt.substr(1).compare("TIME"))==0){
						//long long d_timestamp = atoi(tar_varmap["timestamp"].c_str());
						long long valPrint = 0;
						double trailings = 0.;
						if (rcb.d_timestamp ==0){
							valPrint = Functional::timestamp2seq(8000000);
						}
						else{
							if (rcb.rdtsc_st == 0){
								rcb.rdtsc_st = rd.rt;
								rcb.first_timestamp = rcb.d_timestamp;
								valPrint = rcb.first_timestamp;
							}
							else{
								rcb.diff_rdtsc = rd.rt - rcb.rdtsc_st;
								rcb.accum_rdtsc = (rcb.accum_rdtsc * rcb.filter_ratio) + (rd.rt - rcb.rdtsc_st) * (1-rcb.filter_ratio);
								rcb.accum_timestamp = rcb.accum_timestamp * rcb.filter_ratio + (rcb.d_timestamp - rcb.first_timestamp) * (1-rcb.filter_ratio);
								valPrint = rcb.first_timestamp + (long long)(rcb.diff_rdtsc * ((long double)rcb.accum_timestamp / rcb.accum_rdtsc));
								trailings = (rcb.diff_rdtsc * ((long double)rcb.accum_timestamp / rcb.accum_rdtsc)) - (long long)(rcb.diff_rdtsc * ((long double)rcb.accum_timestamp / rcb.accum_rdtsc));
							}
							rcb.prev_rdtsc = rcb.rdtsc_st;
							rcb.prev_timestamp = rcb.d_timestamp;
							//fprintf(fo,"notime..sorry");
						}
						//target_dates[dfi]
						char fbuf[15] = {0,};
						sprintf_s(fbuf, "%.8f", trailings);
						int rdtstmp = Functional::seq2timestamp((int)valPrint);
						fprintf(fo," %s %08d%s",target_date.c_str(),rdtstmp,fbuf+1);
					}
				}
				else{ prtflag = 1; target_var = fmt; }

				if (prtflag){
					int idx = hdsc.name2ind(target_var);
					int stj = hdsc.dsdiffidx[idx];
					int lenj = hdsc.datasizes[idx];
					char ending_buf[50] = "";
					for (int j=stj; j<stj+lenj ;++j){
						fprintf(fo,"%c",msgbuf[j]);
						ending_buf[j-stj] = msgbuf[j];
					}
					if (target_var_mode == 1){
						tar_varmap[target_var] = ending_buf;
						if (!_strcmpi(target_var.c_str(),"timestamp")){
							rcb.d_timestamp = Functional::timestamp2seq(atoi(ending_buf));
						}
					}
					int intval = atoi(ending_buf);
					if (intval != 0){
						if (!_strcmpi(ref_cols[0][i].c_str(),"price")){
							if (price_var > intval) dir = -1;
							else if (price_var < intval) dir = 1;
							price_var = intval;
						}
						else if (!_strcmpi(ref_cols[0][i].c_str(),"bidPrc1")){
							if (bidPrc1 < intval) dir = 1;
							else if (bidPrc1 > intval) dir = -1;
							if (bidPrc1 != intval) priceChanged = 1;
							bidPrc1 = intval;
						}
						else if (!_strcmpi(ref_cols[0][i].c_str(),"askPrc1")){
							if (askPrc1 < intval) dir = 1;
							else if (askPrc1 > intval) dir = -1;
							if (bidPrc1 != intval) priceChanged = 1;
							askPrc1 = intval;
						}
						else if (!_strcmpi(ref_cols[0][i].c_str(),"bidQty1")){
							if (!priceChanged && bidQty1 > intval){ // buy
								dir = -1;
							}
							bidQty1 = intval;
						}
						else if (!_strcmpi(ref_cols[0][i].c_str(),"askQty1")){
							if (!priceChanged && askQty1 > intval){ // sell
								dir = 1;
							}
							askQty1 = intval;
						}
					}
				}
				if (target_var_mode == 2){
					if (tar_varmap.find(target_var)==tar_varmap.end()){
						fprintf(fo," ");
					}
					else{
						fprintf(fo,"%s",tar_varmap[target_var].c_str());
					}
				}
				fprintf(fo,",");
			}
			fprintf(fo,"\n");
		}

		std::map<std::string, std::string> tar_varmap;
		int askQty1, bidQty1, askPrc1, bidPrc1, dir, priceChanged, price_var;
	};

	struct Rfm{

		void put(const std::string& outputfolder, const std::string& krcode, std::vector<std::vector<std::string> >& ref_cols, int refidx,
			RawDataReader &rd, const char* msgbuf, const std::string& target_date, const HeaderDesc& hdsc)
		{
			if (base.find(krcode) == base.end()){
				base[krcode] = RfmPerCode(outputfolder, krcode, target_date, ref_cols);
			}
			base[krcode].print(ref_cols, refidx, rd, msgbuf, target_date, hdsc, rcb);
		}
		std::map<std::string, RfmPerCode> base;
		RdtscConvBase rcb;
	};

	//(d_target_date, d_datafile, d_outputfolder);
	inline void input_datafiles_output_reformatfile
		(const std::string& target_date, const std::string& datafile, const std::string& outputfolder){

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

		int counter=0;

		RawDataReader rd(datafile);
		Rfm rfm;

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
			if (target_krcode[0] == '*'){
				lenj = hdsc.datasizes[idx];
			}
			else{
				lenj = target_krcode.length();	
			}
			char krcodebuf[50] = "";
			for (int j=stj; j<stj+lenj ;++j){
				krcodebuf[j-stj] = msgbuf[j];
			}
			if (target_krcode[0] == '*');
			else if (_strcmpi(target_krcode.c_str(),krcodebuf)){ // not same
				continue;
			}
			rfm.put(outputfolder, krcodebuf, ref_cols, refidx, rd, msgbuf, target_date, hdsc);

		}
	}

	inline std::string findreplace(const std::string& a_str, const std::string& from, const std::string& to){
		std::string str = a_str;
		std::string keystr = from;
		size_t f = str.find(from);
		str.replace(f, from.length(), to);
		return str;
	}

	inline void reformat_datafiles(){
		std::vector<std::string> target_dates;
		if (target_date!=""){
			target_dates.push_back(target_date);
		}
		else{
			for (Date it_d = target_date_from; it_d != target_date_to+1; it_d = it_d+1){
				target_dates.push_back(it_d.str());
			}
		}
		std::string d_datafile_pre;
		if (datafile!=""){
			d_datafile_pre = datafile;
		}
		else{
			if (*datapath.end()!='\\')
				datapath = datapath + "\\";
			d_datafile_pre = datapath+target_datafile;
		}
		parse_srcfile();
		for (int i=0;i<(int)target_dates.size();++i){
			std::string d_target_date = target_dates[i];
			std::string d_datafile = findreplace(d_datafile_pre,"[$DATE]",d_target_date);
			std::string d_outputfolder = findreplace(outputfolder,"[$DATE]",d_target_date);
			system(("mkdir " + d_outputfolder).c_str());
			input_datafiles_output_reformatfile(d_target_date, d_datafile, d_outputfolder);
		}
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
	std::string datafile;
	std::string outputfolder;
	std::string outputfile;
	std::string wanteddescfile;
	std::string name;
	std::string target_krcode;
	std::string target_date;

	//automation
	std::string datapath;
	std::string target_date_from;
	std::string target_date_to;
	std::string target_datafile;
};

#endif // extractorunit_h__
