#ifndef extractorunit_h__
#define extractorunit_h__

#include <set>
#include <map>
#include "csvparser.hpp"
#include "hashergenerator.hpp"
#include "rawdatareader.hpp"
#include "functional.h"
#include "date.hpp"

#undef UNICODE
#include <Windows.h>

const std::string whiteSpaces( " \f\n\r\t\v" );

inline void trimRight( std::string& str,
			   const std::string& trimChars = whiteSpaces )
{
	std::string::size_type pos = str.find_last_not_of( trimChars );
	str.erase( pos + 1 );    
}


inline void trimLeft( std::string& str,
			  const std::string& trimChars = whiteSpaces )
{
	std::string::size_type pos = str.find_first_not_of( trimChars );
	str.erase( 0, pos );
}


inline void trim( std::string& str, const std::string& trimChars = whiteSpaces )
{
	trimRight( str, trimChars );
	trimLeft( str, trimChars );
}

inline std::vector<std::string> filenames(const std::string& dirpath){
	std::vector<std::string> ret;
	WIN32_FIND_DATA search_data;
	memset(&search_data, 0, sizeof(WIN32_FIND_DATA));
	HANDLE handle = FindFirstFile(dirpath.c_str(), &search_data);
	while(handle != INVALID_HANDLE_VALUE)
	{
		ret.push_back(search_data.cFileName);
		if(FindNextFile(handle, &search_data) == FALSE)
			break;
	}
	//Close the handle after use or memory/resource leak
	FindClose(handle);
	return ret;
}

inline void check_and_create_directory(const std::string& dirpath){
	std::vector<std::string> files = filenames(dirpath + "*");
	if (files.size()==0)
		system(("mkdir " + dirpath).c_str());
}

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
		reconfigure,
		reconfigure_detail,
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
		long double est_rdtstmp, prev_max_est_rdtsmp;
		long double filter_ratio, accum_rdtsc, accum_timestamp;
		long long nSamples;
		RdtscConvBase(){
			rdtsc_st = 0, diff_rdtsc = 0, prev_rdtsc = 0, prev_timestamp = 0, first_timestamp = 0, d_timestamp = 0;
			est_rdtstmp = prev_max_est_rdtsmp = 0;
			filter_ratio = 0;
			nSamples = 0;
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
								rcb.filter_ratio = (long double)rcb.nSamples / (1 + rcb.nSamples);
								++rcb.nSamples;
								rcb.est_rdtstmp = (long double)rcb.accum_timestamp / rcb.accum_rdtsc * rcb.diff_rdtsc;
								if (rcb.est_rdtstmp < rcb.prev_max_est_rdtsmp+1e-6)
									rcb.est_rdtstmp = rcb.prev_max_est_rdtsmp+1e-6;
								long double est_rdtstmp = rcb.est_rdtstmp;
								valPrint = rcb.first_timestamp + (long long)(est_rdtstmp);
								trailings = (est_rdtstmp) - (long long)(est_rdtstmp);
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
						if (rcb.prev_max_est_rdtsmp < rcb.est_rdtstmp)
							rcb.prev_max_est_rdtsmp = rcb.est_rdtstmp;
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
						std::string key = ref_cols[0][i];
						trim(key);
						if (!_strcmpi(key.c_str(),"price")){
							/* old askqty1*/
							/* old bidqty1*/
							if (askPrc1 == intval){
								dir = 1;
								askPrc1 += 5;
							}
							else if (bidPrc1 == intval){
								dir = -1;
								bidPrc1 -= 5;
							}
							else{
								if (price_var > intval) dir = -1;
								else if (price_var < intval) dir = 1;
							}
							price_var = intval;
						}
						else if (!_strcmpi(key.c_str(),"bidPrc1")){
							if (bidPrc1 < intval) dir = 1;
							else if (bidPrc1 > intval) dir = -1;
							if (bidPrc1 != intval) priceChanged = 1;
							bidPrc1 = intval;
						}
						else if (!_strcmpi(key.c_str(),"askPrc1")){
							if (askPrc1 < intval) dir = 1;
							else if (askPrc1 > intval) dir = -1;
							if (bidPrc1 != intval) priceChanged = 1;
							askPrc1 = intval;
						}
						else if (!_strcmpi(key.c_str(),"bidQty1")){
							if (!priceChanged && bidQty1 > intval){ // buy
								dir = -1;
							}
							bidQty1 = intval;
						}
						else if (!_strcmpi(key.c_str(),"askQty1")){
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
					std::string inp = descin.getstr();
					col.push_back(inp);
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
		if (f==-1)
			return str;
		str.replace(f, from.length(), to);
		return str;
	}

	
	struct ObSig{
		ObSig() : type(), dir(0), bidqty1(0), askqty1(0), bidprc1(0), askprc1(0) {}
		std::string type;
		int dir, bidqty1, askqty1, bidprc1, askprc1;
	};

	struct BAbook{
		BAbook() { memset(prc,0,sizeof(prc)); memset(qty,0,sizeof(qty)); }
		int prc[5], qty[5];
		bool initialized() const{
			return prc[0]>0 || prc[1]>0 || prc[2]>0 || prc[3]>0 || prc[4]>0;
		}
	};

	struct ObSigN{
		ObSigN() : type(), dir(0), bbook(), abook() {}
		std::string type;
		int dir, price;
		BAbook bbook, abook;
		std::pair<int, int> prc2book(int price) const{
			for (int i=0;i<2;++i){
				const BAbook *target;
				if (i==0) target = &bbook;
				else target = &abook;
				for (int j=0;j<5;++j){
					if (target->prc[j] == price){
						// j<0 then bid, j>0 then ask
						return std::make_pair((i==0) ? -(j+1) : (j+1), target->qty[j]);
					}
				}
			}
			return std::make_pair(0,0);
		}
		std::vector<int> prices() const{
			std::vector<int> ret;
			for (int i=0;i<2;++i)
				for (int j=0;j<5;++j)
					ret.push_back((i==0) ? bbook.prc[j] : abook.prc[j]);
			return ret;
		}
		void remove_price(int price){
			std::pair<int, int> prcpair = prc2book(price);
			BAbook *target = NULL;
			if (prcpair.first < 0) target = &bbook;
			else target = &abook;
			for (int i=0;i<5;++i){
				target->prc[i] = target->prc[i+1];
				target->qty[i] = target->qty[i+1];
			}
			target->prc[4] = target->prc[3] + (target->prc[3] - target->prc[2]);
			target->qty[4] = -1;
		}
	};

	template <typename T> int sign(T val) {
		return (T(0) < val) - (val < T(0));
	}

	inline void reconfigure_file(const std::string& fromfile, const std::string& tofile, int detail_verbose){
		CsvParser csvp(fromfile);
		csvp.getline();
		std::vector<std::string> header = csvp.line;
		int ti;
		std::vector<int> is;
		FILE *fo = NULL;
		fopen_s(&fo, tofile.c_str(), "wt");

		BAbook ibbook, iabook;
		int idir, ivol, iprice;
		iprice = ivol = idir = 0;

		for (int i=0;i<(int)header.size();++i){
			int isprtlist = 0;
			if (!_strcmpi(header[i].c_str(),"type")){
				ti = i;
				isprtlist = 1;
			}
			else{
				if (detail_verbose) isprtlist = 1;
				else if (!_strcmpi(header[i].c_str(),"arrivaltime") || !_strcmpi(header[i].c_str(),"price") || !_strcmpi(header[i].c_str(),"vol") || !_strcmpi(header[i].c_str(),"direction")
					|| !_strcmpi(header[i].c_str()," bidPrc1") || !_strcmpi(header[i].c_str(),"bidQty1") || !_strcmpi(header[i].c_str()," AskPrc1") || !_strcmpi(header[i].c_str(),"AskQty1")){
						isprtlist = 1;
				}
			}
			if (isprtlist){
				fprintf(fo,"%s,",header[i].c_str());
				is.push_back(i);
			}

			if (!_strcmpi(header[i].c_str(),"direction")) idir = i;
			if (!_strcmpi(header[i].c_str(),"vol")) ivol = i;
			if (!_strcmpi(header[i].c_str(),"price")) iprice = i;
			std::string key = header[i];
			if (key[0] == ' ') key = key.substr(1); // trim.
			BAbook *targetbook = NULL;
			if (!_strcmpi(key.substr(0,3).c_str(),"bid")){ targetbook = &ibbook; }
			else if (!_strcmpi(key.substr(0,3).c_str(),"ask")){ targetbook = &iabook; }
			if (targetbook){
				key = key.substr(3);
				int *targetarr = NULL;
				if (!_strcmpi(key.substr(0,3).c_str(),"qty")){ targetarr = targetbook->qty; }
				else if (!_strcmpi(key.substr(0,3).c_str(),"prc")){ targetarr = targetbook->prc; }
				if (targetarr){
					key = key.substr(3);
					int idx = atoi(key.c_str());
					targetarr[idx-1] = i;
				}
			}
		}
		int price = 0, vol = 0, seq = 0;
		ObSigN oldsig, sig;
		fprintf(fo,"type1,price1,vol1,type2,price2,vol2,type3,price3,vol3,\n");
		while(csvp.getline()){

			std::vector<std::string> content = csvp.line;

			for (int i=0;i<(int)is.size();++i){
				if (is[i] < (int)content.size())
					fprintf(fo,"%s,",content[is[i]].c_str());
				else
					fprintf(fo," ,");
			}

			price = atoi(content[iprice].c_str());
			vol = atoi(content[ivol].c_str());
			seq = atoi(content[0].c_str());
			if (seq == 35303){
				seq = seq;
			}
			sig.type = content[ti];
			sig.dir = atoi(content[idir].c_str());
			sig.price = price;
			if (!_strcmpi(sig.type.c_str(),"BA") || !_strcmpi(sig.type.c_str(),"TBA")){
				for (int i=0;i<2;++i){
					BAbook *targetb, *idxb;
					if (i==0) targetb = &sig.bbook, idxb = &ibbook;
					else targetb = &sig.abook, idxb = &iabook;
					for (int j=0;j<2;++j){
						int *target, *idx;
						if (j==0) target = targetb->qty, idx = idxb->qty;
						else target = targetb->prc, idx = idxb->prc;
						for (int k=0;k<5;++k)
							target[k] = atoi(content[idx[k]].c_str());
					}
				}
			}

			std::vector<int> prcs;
			std::vector<std::pair<int, int> > osbs, nsbs;
			std::pair<int, int> osb, nsb;

			if (oldsig.type.length()){
				std::vector<int> nprices = sig.prices();
				std::vector<int> oprices = oldsig.prices();
				nprices.insert(nprices.end(),oprices.begin(),oprices.end());
				std::sort(nprices.begin(),nprices.end());
				nprices.resize(std::unique(nprices.begin(),nprices.end())-nprices.begin());
				for (int i=0;i<(int)nprices.size();++i){
					std::pair<int, int> sb = sig.prc2book(nprices[i]);
					std::pair<int, int> oldsb = oldsig.prc2book(nprices[i]);
					if (sb.second != oldsb.second && (oldsb.second != -1)){ // if -1 then (T, BA) or (T, TBA)
						if ((sb.second==0 && (oldsb.first==5 || oldsb.first==-5)) || // 5 disappears
							(oldsb.second==0 && (sb.first==5 || sb.first==-5)) // 5 comes
							)
							continue;
						//if (oldsb.second==0 && oldsig.price == nprices[i])
						//	continue;
						nsbs.push_back(sb);
						osbs.push_back(oldsb);
						prcs.push_back(nprices[i]);
					}
				}
			}
			if (nsbs.size()==3){
				int j = 1;
			}
			std::string typestr[4] = {"","trade","cancel","insertion"};
			int type12[9] = {0,}, price12[9] = {0,}, vol12[9] = {0,}, cc = 0;
			if (!_strcmpi(sig.type.c_str(),"TBA") || !_strcmpi(sig.type.c_str(),"T")){ // at least it's traded.
				type12[cc] = 1; // trade;
				price12[cc] = price;
				vol12[cc++] = vol;
				for (int i=0;i<(int)osbs.size();++i){
					if (prcs[i] == price){
						if (sign(nsbs[i].first) == sig.dir){
							osbs.erase(osbs.begin()+i);
							prcs.erase(prcs.begin()+i);
							nsbs.erase(nsbs.begin()+i);
						}
						else{
							osbs[i].second = 0;
						}
						break;
					}
				}
			}
			if (!_strcmpi(sig.type.c_str(),"TBA") || !_strcmpi(sig.type.c_str(),"BA")){
				if (oldsig.type.length() && oldsig.abook.initialized() && oldsig.bbook.initialized() && sig.abook.initialized() && sig.bbook.initialized()){
					for (int i=0;i<(int)osbs.size();++i){
						if (nsbs[i].second > osbs[i].second){
							type12[cc] = 3; // insertion
							price12[cc] = prcs[i];
							vol12[cc] = nsbs[i].second - osbs[i].second;
							++cc;
							//break;
						}
						if (nsbs[i].second < osbs[i].second){
							type12[cc] = 2; // cancellation
							price12[cc] = prcs[i];
							vol12[cc] = osbs[i].second - nsbs[i].second;
							++cc;
							//break;
						}
					}
				}
			}
			if (cc>3){
				printf("sung\n");
				cc = cc;
			}
			if (!_strcmpi(sig.type.c_str(),"T")){
				sig.bbook = oldsig.bbook;
				sig.abook = oldsig.abook;
				sig.remove_price(price);
			}
			for (int i=0;i<3;++i){
				if (cc>i) fprintf(fo,"%s,%d,%d,",typestr[type12[i]].c_str(),price12[i],vol12[i]);
				else fprintf(fo,",,,");
			}
			fprintf(fo,"\n");
			
			oldsig = sig;
		}
	}

	inline void reconfigure_file_old(const std::string& fromfile, const std::string& tofile, int detail_verbose){
		CsvParser csvp(fromfile);
		csvp.getline();
		std::vector<std::string> header = csvp.line;
		int ti;
		std::vector<int> is;
		FILE *fo = NULL;
		fopen_s(&fo, tofile.c_str(), "wt");

		int idir, ibqty1, iaqty1, ibprc1, iaprc1, ivol;
		ivol = idir = ibqty1 = iaqty1 = ibprc1 = iaprc1 = 0;

		for (int i=0;i<(int)header.size();++i){
			if (!_strcmpi(header[i].c_str(),"type")){
				ti = i;
			}
			else{
				int isprtlist = 0;
				if (detail_verbose) isprtlist = 1;
				else if (!_strcmpi(header[i].c_str(),"arrivaltime") || !_strcmpi(header[i].c_str(),"price") || !_strcmpi(header[i].c_str(),"vol") || !_strcmpi(header[i].c_str(),"direction")
					|| !_strcmpi(header[i].c_str()," bidPrc1") || !_strcmpi(header[i].c_str(),"bidQty1") || !_strcmpi(header[i].c_str()," AskPrc1") || !_strcmpi(header[i].c_str(),"AskQty1")){
					isprtlist = 1;
				}
				if (isprtlist){
					fprintf(fo,"%s,",header[i].c_str());
					is.push_back(i);
				}
			}

			if (!_strcmpi(header[i].c_str(),"direction")) idir = i;
			if (!_strcmpi(header[i].c_str(),"vol")) ivol = i;
			if (!_strcmpi(header[i].c_str(),"bidQty1")) ibqty1 = i;
			if (!_strcmpi(header[i].c_str(),"askQty1")) iaqty1 = i;
			if (!_strcmpi(header[i].c_str()," bidPrc1")) ibprc1 = i;
			if (!_strcmpi(header[i].c_str()," askPrc1")) iaprc1 = i;
		}

		if (detail_verbose)
			fprintf(fo,"is_level1event,");

		ObSig oldsig, sig;
		fprintf(fo,"is_sell,is_buy,is_bidinsertion,is_askinsertion,is_bidcancelled,is_askcancelled,dir_bidmoved,dir_askmoved,");

		fprintf(fo,"\n");
		while(csvp.getline()){
			std::vector<std::string> content = csvp.line;
			//if (!_strcmpi(content[ti].c_str(),"T") || !_strcmpi(content[ti].c_str(),"TBA") ){

			int dirtraded;
			int askqtydelta1, bidqtydelta1;
			int dirbidmoved, diraskmoved;
			dirtraded = askqtydelta1 = bidqtydelta1 = dirbidmoved = diraskmoved = 0;

			sig.type = content[ti];
			sig.dir = atoi(content[idir].c_str());
			sig.askqty1 = atoi(content[iaqty1].c_str()); sig.askprc1 = atoi(content[iaprc1].c_str());
			sig.bidqty1 = atoi(content[ibqty1].c_str()); sig.bidprc1 = atoi(content[ibprc1].c_str());
			int qtytraded = atoi(content[ivol].c_str());

			dirtraded = sig.dir;
			if (!_strcmpi(sig.type.c_str(),"BA") || !_strcmpi(sig.type.c_str(),"TBA")){
				// BA나 TBA일 때에.. ask 또는 bid 1호가 가격이 바뀐 경우 각각의 direction을 계산 가능하다.
				diraskmoved = sign(sig.askprc1 - oldsig.askprc1);
				dirbidmoved = sign(sig.bidprc1 - oldsig.bidprc1);
				// askqty1 이 -1이라는 것은 T로 한 가격 밀린 호가의 qty는 알 수 없다는(알 필요 없다는) 의미이다.
				// 이 부분에서 insertion, deletion 일어날 수 없기 때문.
				// ask1이 움직이지 않은 상태라면 insertion, deletion delta를 계산 가능하다.
				if (oldsig.askqty1 != -1 && !diraskmoved){
					// askqtydelta1은 ask 1호가의 insertion, deletion volume을 의미한다.
					askqtydelta1 = sig.askqty1 - oldsig.askqty1;
					// 체결된 수량만큼 insertion, deletion 델타에서 제외한다.
					if (sig.dir>0) askqtydelta1 += qtytraded;
				}
				// bid part도 마찬가지로 한다.
				if (oldsig.bidqty1 != -1 && !dirbidmoved){
					bidqtydelta1 = sig.bidqty1 - oldsig.bidqty1;
					if (sig.dir<0) bidqtydelta1 += qtytraded;
				}
				// ask 1호가가 - 방향으로 일어난 경우 새로운 price가 order book에 quote로 쌓인 것이다.
				if (diraskmoved < 0) askqtydelta1 = sig.askqty1;
				// bid 역시 마찬가지로 해 준다.
				if (dirbidmoved > 0) bidqtydelta1 = sig.bidqty1;
			}
			if (!_strcmpi(sig.type.c_str(),"T")){
				if (dirtraded>0){
					diraskmoved = 1;
					//T 인 경우 예컨대 buy 인 경우 ask는 실종되고 bid는 남는다.
					sig.bidprc1 = oldsig.bidprc1; // bid1 remains 
					sig.bidqty1 = oldsig.bidqty1;
					// ask는 실종됨.
					sig.askprc1 += 5;
					sig.askqty1 = -1; // don't know
				}
				if (dirtraded<0){
					dirbidmoved = -1;
					sig.askprc1 = oldsig.askprc1; // ask1 remains
					sig.askqty1 = oldsig.askqty1;
					sig.bidprc1 -= 5;
					sig.bidqty1 = -1; // don't know

				}
			}
			
			oldsig = sig;

			if (detail_verbose){
				for (int i=0;i<(int)is.size();++i){
					if (is[i] < (int)content.size())
						fprintf(fo,"%s,",content[is[i]].c_str());
					else
						fprintf(fo," ,");
				}
				if (dirtraded || diraskmoved || dirbidmoved || askqtydelta1 || bidqtydelta1)
				{
					fprintf(fo,"1,");
				}
				else{
					fprintf(fo,"0,");
				}
				fprintf(fo,"%d,%d,",dirtraded==-1,dirtraded==1);
				fprintf(fo,"%d,%d,%d,%d,",bidqtydelta1>0,askqtydelta1>0,bidqtydelta1<0,askqtydelta1<0);
				fprintf(fo,"%d,%d,",dirbidmoved,diraskmoved);
				fprintf(fo,"\n");
			}
			else if (dirtraded || diraskmoved || dirbidmoved || askqtydelta1 || bidqtydelta1){
				for (int i=0;i<(int)is.size();++i){
					fprintf(fo,"%s,",content[is[i]].c_str());
				}
				fprintf(fo,"%d,%d,",dirtraded==-1,dirtraded==1);
				fprintf(fo,"%d,%d,%d,%d,",bidqtydelta1>0,askqtydelta1>0,bidqtydelta1<0,askqtydelta1<0);
				fprintf(fo,"%d,%d,",dirbidmoved,diraskmoved);
				fprintf(fo,"\n");
			}
		}
		fclose(fo);
	}

	inline void reconfigure_files(){
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
			if (datapath[(int)datapath.size()-1]!='\\')
				datapath = datapath + "\\";
			d_datafile_pre = datapath+target_datafile;
		}
		for (int i=0;i<(int)target_dates.size();++i){
			std::string d_target_date = target_dates[i];
			std::string d_datapath = findreplace(d_datafile_pre,"[$DATE]",d_target_date);
			std::string d_dirpath = findreplace(datapath,"[$DATE]",d_target_date);
			std::string d_outputfolder = findreplace(outputfolder,"[$DATE]",d_target_date);

			std::vector<std::string> datafiles = filenames(d_datapath);
			for (int i=0;i<(int)datafiles.size();++i){
				if (datafiles[i][0]!='.'){
					//printf("%s\n",datafiles[i].c_str());
					std::string fromfile = d_dirpath + datafiles[i];
					std::string tofile = d_outputfolder + datafiles[i];
					//printf("[%s] -> [%s]\n",fromfile.c_str(),tofile.c_str());
					check_and_create_directory(d_outputfolder);
					reconfigure_file(fromfile, tofile, (type == reconfigure_detail) );
				}
			}
		}
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
			if (datapath[(int)datapath.size()-1]!='\\')
				datapath = datapath + "\\";
			d_datafile_pre = datapath+target_datafile;
		}
		parse_srcfile();
		for (int i=0;i<(int)target_dates.size();++i){
			std::string d_target_date = target_dates[i];
			std::string d_datafile = findreplace(d_datafile_pre,"[$DATE]",d_target_date);
			std::string d_outputfolder = findreplace(outputfolder,"[$DATE]",d_target_date);
			check_and_create_directory(d_outputfolder);
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
		else if (type == reconfigure || type == reconfigure_detail){
			reconfigure_files();
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
