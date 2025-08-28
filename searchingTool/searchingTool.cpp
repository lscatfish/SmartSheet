
#include <basic.hpp>
#include <Encoding.h>
#include <Files.h>
#include <helper.h>
#include <iostream>
#include <pdf.h>
#include <searchingTool/searchingTool.hpp>
#include <string>
#include <xlnt/workbook/workbook.hpp>



//// 解析xlsx
//void SearchingTool::parse_xlsx_list( ) {
//    list< std::string > u8PathList  = this->get_u8filepath_list(list< std::string >{ ".xlsx" });
//    list< std::string > sysPathList = this->get_filepath_list(list< std::string >{ ".xlsx" });
//
//    for (size_t i = 0; i < sysPathList.size( ); i++) {
//        TextList< xlnt::workbook > afile(sysPathList[i], u8PathList[i]);
//        xlsxList_.push_back(afile);
//    }
//}
//
//// 解析pdf
//void SearchingTool::parse_pdf_list( ) {
//    list< std::string > u8PathList  = this->get_u8filepath_list(list< std::string >{ ".pdf", ".PDF" });
//    list< std::string > sysPathList = this->get_filepath_list(list< std::string >{ ".xlsx", ".PDF" });
//
//    for (size_t i = 0; i < sysPathList.size( ); i++) {
//        TextList< pdf::DefPdf > afile(sysPathList[i], u8PathList[i]);
//        pdfList_.push_back(afile);
//    }
//}
//
//// 解析docx
//void SearchingTool::parse_docx_list( ) {
//    list< std::string > u8PathList  = this->get_u8filepath_list(list< std::string >{ ".pdf", ".PDF" });
//    list< std::string > sysPathList = this->get_filepath_list(list< std::string >{ ".xlsx", ".PDF" });
//
//    for (size_t i = 0; i < sysPathList.size( ); i++) {
//        TextList< pdf::DefPdf > afile(sysPathList[i], u8PathList[i]);
//        pdfList_.push_back(afile);
//    }
//}
