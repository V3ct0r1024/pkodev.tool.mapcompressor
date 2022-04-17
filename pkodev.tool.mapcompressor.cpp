#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <vector>
#include <memory>

namespace pkodev
{
	// Some useful constants
	const unsigned int magic{ 0x000BE953 };
	const unsigned int max_map_width{ 4096 };
	const unsigned int max_map_height{ 4096 };
	const unsigned int max_section_width{ 8 };
	const unsigned int max_section_height{ 8 };
	const unsigned int tile_size{ 15 };
	const unsigned int tiles_in_section = max_section_width * max_section_height;
	const unsigned int section_size{ tiles_in_section * tile_size };

	// Map header
	struct map_header
	{
		unsigned int magic;          // Magic number
		unsigned int width;          // Map width
		unsigned int height;         // Map height
		unsigned int section_width;  // Map section width
		unsigned int section_height; // Map section height

		// Constructor
		map_header() :
			magic(0),
			width(0),
			height(0),
			section_width(0),
			section_height(0)
		{

		}
	};

	// Map section structure
	struct map_section
	{
		unsigned char tiles[section_size]; // Tiles data

		// Constructor
		map_section()
		{
			std::memset(reinterpret_cast<void*>(tiles), 0x00, section_size);
		}
	};

}

// Entry point
int main(int argc, char *argv[])
{
	// Print welcome message
	std::cout << "PKOdev.NET .map file compressor program" << std::endl;
	std::cout << "Version: v1.0.0 (03/01/2022)" << std::endl;
	std::cout << "Author: V3ct0r" << std::endl << std::endl;
	
	// Trim string
	auto trim = [](const std::string& str) -> std::string
	{
		// Space characters
		const std::string whitespace = " \n\r\t\f\v";

		// Left trim
		auto left_trim = [&whitespace](const std::string& str) -> std::string
		{
			const std::size_t pos = str.find_first_not_of(whitespace);
			return (pos == std::string::npos) ? "" : str.substr(pos);
		};

		// Right trim
		auto right_trim = [&whitespace](const std::string& str) -> std::string
		{
			const std::size_t pos = str.find_last_not_of(whitespace);
			return (pos == std::string::npos) ? "" : str.substr(0, pos + 1);
		};
		
		// Trim the string
		return right_trim(left_trim(str));
	};

	// Extract file name from full path
	auto extract_file_name = [](const std::string& path) -> std::string
	{
		const std::size_t pos = path.find_last_of("/\\");
		if (pos == std::string::npos) 
		{
			return path;
		}

		return path.substr(pos + 1);
	};
	
	// Check parameters count
	if (argc != 3)
	{
		// Print usage
		std::cout << "Usage: " << extract_file_name(trim(std::string(argv[0]))) << " <input .map file> <output .map file>";
		return 1;
	}

	// Parse parameters
	const std::string path_in = trim(std::string(argv[1]));
	const std::string path_out = trim(std::string(argv[2]));

	// Print parameters
	std::cout << "Input .map file: " << extract_file_name(path_in) << std::endl;
	std::cout << "Output .map file: " << extract_file_name(path_out) << std::endl << std::endl;

	// Open the input .map file
	std::ifstream in(path_in, std::ios::in | std::ios::binary);

	// Check that the input file is open
	if (in.is_open() == false)
	{
		// Error
		std::cout << "[Error] Can't open the .map file " << extract_file_name(path_in) << "!" << std::endl;
		return 1;
	}

	// Map header
	pkodev::map_header header;

	// Read magic number
	in.read(reinterpret_cast<char*>(&header.magic), sizeof(header.magic));
	
	// Check the magic number
	if (pkodev::magic != header.magic)
	{
		// Unknown file format
		std::cout << "[Error] Wrong the .map file format " << extract_file_name(path_in) << "!" << std::endl;
		return 1;
	}

	// Read header
	in.read(reinterpret_cast<char*>(&header.width), sizeof(header.width));
	in.read(reinterpret_cast<char*>(&header.height), sizeof(header.height));
	in.read(reinterpret_cast<char*>(&header.section_width), sizeof(header.section_width));
	in.read(reinterpret_cast<char*>(&header.section_height), sizeof(header.section_height));

	// Check map width
	if ( (header.width < header.section_width) || (header.width > pkodev::max_map_width) )
	{
		// Wrong map width
		std::cout << "[Error] Wrong the " << extract_file_name(path_in) << " .map file map width!" << std::endl;
		return 1;
	}

	// Check map height
	if ( (header.height < header.section_height) || (header.height > pkodev::max_map_height) )
	{
		// Wrong map height
		std::cout << "[Error] Wrong the " << extract_file_name(path_in) << " .map file map height!" << std::endl;
		return 1;
	}

	// Check section width
	if (pkodev::max_section_width != header.section_width)
	{
		// Wrong section width
		std::cout << "[Error] Wrong the " << extract_file_name(path_in) << " .map file section width!" << std::endl;
		return 1;
	}

	// Check section height
	if (pkodev::max_section_height != header.section_height)
	{
		// Wrong section height
		std::cout << "[Error] Wrong the " << extract_file_name(path_in) << " .map file section height!" << std::endl;
		return 1;
	}

	// Calculate section number
	const unsigned int section_num = (header.width / header.section_width)
		* (header.height / header.section_height);

	// Calculate sections table size
	const unsigned int table_size = section_num * sizeof(unsigned int);

	// Print header
	std::cout << "Input .map file information:" << std::endl;
	std::cout << "* Width: " << header.width << std::endl;
	std::cout << "* Height: " << header.height << std::endl;
	std::cout << "* Section width: " << header.section_width << std::endl;
	std::cout << "* Section height: " << header.section_height << std::endl;
	std::cout << "* Sections number: " << section_num << std::endl << std::endl;

	// Open the output .map file
	std::ofstream out(path_out, std::ios::out | std::ios::binary);

	// Check that the output .map file is open
	if (out.is_open() == false)
	{
		// Error
		std::cout << "[Error] Can't create the .map file " << extract_file_name(path_out) << "!" << std::endl;
		return 1;
	}

	// Write file header
	out.write(reinterpret_cast<char*>(&header.magic), sizeof(header.magic));
	out.write(reinterpret_cast<char*>(&header.width), sizeof(header.width));
	out.write(reinterpret_cast<char*>(&header.height), sizeof(header.height));
	out.write(reinterpret_cast<char*>(&header.section_width), sizeof(header.section_width));
	out.write(reinterpret_cast<char*>(&header.section_height), sizeof(header.section_height));

	// Number of sections to be written to the compressed .map file
	unsigned int n = 0;

	// Number of empty sections
	unsigned empty_counter = 0;

	// Current offset
	unsigned int offset = 0;

	// A list of sections
	std::vector< pkodev::map_section > sections;

	// Current map section
	pkodev::map_section section;

	// Write 'wait' message
	std::cout << "Please wait . . . " << std::endl << std::endl;

	// Compressing . . .
	try
	{
		// Sections table pointer
		auto sections_table = std::make_unique<int[]>(section_num);

		// Reserve some memory for sections
		sections.reserve(section_num / 2);

		// Read sections from input .map file
		for (unsigned int i = 0; i < section_num; ++i)
		{
			// Read offset to section
			in.read(reinterpret_cast<char*>(&offset), sizeof(offset));

			// Check offset
			if (offset != 0)
			{
				// Current offset
				const std::streampos cur = in.tellg();

				// Go to offset
				in.seekg(offset, in.beg);

				// Read section tiles
				in.read(reinterpret_cast<char*>(section.tiles), pkodev::section_size);

				// Go back
				in.seekg(cur, in.beg);

				// Analyze the section
				{
					// Empty section flag
					bool empty = false;

					// Loop over tiles
					for (unsigned int j = 0; j < pkodev::tiles_in_section; ++j)
					{
						// 'btTileInfo' field from SNewFileTile structure
						const unsigned char btTileInfo = static_cast<unsigned char>(
							section.tiles[j * pkodev::tile_size + 0x03]
						);

						// 'sRegion' field from SNewFileTile structure
						const unsigned short int sRegion = static_cast<unsigned short int>(
							section.tiles[j * pkodev::tile_size + 0x08]
						);

						// Check that current tile is empty
						if ( (btTileInfo != 0) || ((sRegion & 0x0001) || (sRegion & 0x0008)) )
						{
							// The tile is not empty
							empty = false;
							break;
						}

						// Increase the empty tiles number
						empty = true;
					}

					// Check empty tiles
					if (empty == true)
					{
						// Set section as empty
						sections_table[i] = 0;

						// Increment empty sections counter
						++empty_counter;
					}
					else
					{
						// Calculate the section offset
						sections_table[i] = 20 + table_size + (n++) * pkodev::section_size;

						// Add current section to the list
						sections.push_back(section);
					}
				}
			}
		}

		// Write section table to the compressed .map file
		out.write(reinterpret_cast<const char*>(sections_table.get()), table_size);

		// Write sections to the compressed .map file
		for (unsigned int i = 0; i < n; ++i)
		{
			// Write section
			out.write(reinterpret_cast<const char*>(sections[i].tiles), pkodev::section_size);
		}
	}
	catch (const std::bad_alloc& e)
	{
		// Not enough memory
		std::cout << "[Error] Not enough memory: " << e.what() << std::endl;
		return 1;
	}
	catch (...)
	{
		// Not enough memory
		std::cout << "[Error] Unknown error!" << std::endl;
		return 1;
	}

	// Close the input .map file
	in.close();

	// Close the output .map file
	out.close();

	// Calculate some statistics
	const float percentage = (static_cast<float>(empty_counter) / static_cast<float>(section_num)) * 100.0f;
	const unsigned int bytes = (empty_counter * pkodev::section_size);

	// Print statistics
	std::cout << "Done!" << std::endl;
	std::cout << "* Sections compressed: " << empty_counter << std::endl;
	std::cout << std::setprecision(2) << "* Percentage: " << percentage << "%" << std::endl;
	std::cout << "* Bytes freed: " << bytes << std::endl;

	return 0;
}
