#include "utilities.h"
#include <Psapi.h>

c_globals g_globals;

typedef struct _IMAGE_DOS_HEADERS {      // DOS .EXE header
	WORD   e_magic;                     // Magic number
	WORD   e_cblp;                      // Bytes on last page of file
	WORD   e_cp;                        // Pages in file
	WORD   e_crlc;                      // Relocations
	WORD   e_cparhdr;                   // Size of header in paragraphs
	WORD   e_minalloc;                  // Minimum extra paragraphs needed
	WORD   e_maxalloc;                  // Maximum extra paragraphs needed
	WORD   e_ss;                        // Initial ( relative ) SS value
	WORD   e_sp;                        // Initial SP value
	WORD   e_csum;                      // Checksum
	WORD   e_ip;                        // Initial IP value
	WORD   e_cs;                        // Initial ( relative ) CS value
	WORD   e_lfarlc;                    // File address of relocation table
	WORD   e_ovno;                      // Overlay number
	WORD   e_res[4];                    // Reserved words
	WORD   e_oemid;                     // OEM identifier ( for e_oeminfo )
	WORD   e_oeminfo;                   // OEM information; e_oemid specific
	WORD   e_res2[10];                  // Reserved words
	LONG   e_lfanew;                    // File address of new exe header
} IMAGE_DOS_HEADERS, * PIMAGE_DOS_HEADERS;

uintptr_t c_globals::sig_scan( const char* module, const char* pattern )
{
	uintptr_t module_address = reinterpret_cast< uintptr_t >( GetModuleHandleA( module ) );

	static auto pattern_2byte = []( const char* pattern )
	{
		auto       bytes = std::vector<int>{};
		const auto start = const_cast<char*>( pattern );
		const auto end = const_cast<char*>( pattern ) + strlen( pattern );

		for ( auto current = start; current < end; ++current )
		{
			if ( *current == '?' )
			{
				++current;
				if ( *current == '?' )
					++current;
				bytes.push_back( -1 );
			}
			else { bytes.push_back( strtoul( current, &current, 16 ) ); }
		}
		return bytes;
	};

	const auto dos_header = ( PIMAGE_DOS_HEADERS )module_address;
	if( !dos_header )
		return NULL;

	const auto nt_headers = ( PIMAGE_NT_HEADERS )( ( std::uint8_t* )module_address + dos_header->e_lfanew );

	const auto image_size = nt_headers->OptionalHeader.SizeOfImage;
	auto       pattern_bytes = pattern_2byte( pattern );
	const auto scan_bytes = reinterpret_cast<std::uint8_t*>( module_address );

	const auto s = pattern_bytes.size( );
	const auto d = pattern_bytes.data( );

	for ( auto i = 0ul; i < image_size - s; ++i )
	{
		bool found = true;
		for ( auto j = 0ul; j < s; ++j )
		{
			if ( scan_bytes[i + j] != d[j] && d[j] != -1 )
			{
				found = false;
				break;
			}
		}
		if ( found ) { 
			return reinterpret_cast< uintptr_t >( &scan_bytes[ i ] );
			break;
		}
	}
	return NULL;
}