#pragma once

#include "H5Cpp.h"

class CHDF5DataManager {
public:
  CHDF5DataManager(void);
  ~CHDF5DataManager(void);
  H5::H5File *GetH5File( void ) { return &m_H5File; };
  void AddGroup( const std::string &sGroup );
  void AddGroupForSymbol( const std::string &sSymbol );
  static herr_t PrintH5ErrorStackItem( int n, H5E_error_t *err_desc, void *client_data );
  static hsize_t H5ChunkSize( void ) { return 64; };
protected:
  static const char m_H5FileName[];
  static unsigned int m_RefCount;
  static H5::H5File m_H5File;
private:
};