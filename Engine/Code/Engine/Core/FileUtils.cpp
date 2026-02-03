#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

//-----------------------------------------------------------------------------------------------
int FileReadToBuffer( std::vector<uint8_t>& outBuffer, const std::string& filename )
{
	FILE* fp;
	errno_t errorCode;
	if ( ( errorCode = fopen_s( &fp, filename.c_str(), "rb") ) == 0 )
	{
		fseek(fp, 0, SEEK_END);
		int fileSize = ftell(fp);
		if ( fileSize > 0 )
		{
			outBuffer.resize( fileSize );
			rewind( fp );
			fread( outBuffer.data(), 1, fileSize, fp );
			fclose( fp );
			return fileSize;
		}
		else
		{
			ERROR_AND_DIE( "FileReadToBuffer: File cannot be empty" );
		}
		
	}
	else
	{
		ERROR_AND_DIE("FileReadToBuffer: File does not exist");
	}
}

//-----------------------------------------------------------------------------------------------
int FileReadToString( std::string& outString, const std::string& filename )
{
	std::vector<uint8_t> outBuffer;
	FileReadToBuffer(outBuffer, filename);
	outBuffer.push_back('\0');
	outString = std::string( outBuffer.begin(), outBuffer.end() );
	return static_cast<int>(outString.size());
}
