#include "map_compressor.h"
#include <fstream>

namespace pkodev
{
	// Constructor
	map_compressor::map_compressor() :
		m_magic(magic),
		m_width(0),
		m_height(0),
		m_section_width(max_section_width),
		m_section_height(max_section_height)
	{

	}

	// Destructor
	map_compressor::~map_compressor()
	{

	}

	// Open .map file
	void map_compressor::open(const std::string& path)
	{
		// Remove old sections
		m_sections.clear();

		// Open a file
		std::ifstream file(path, std::ifstream::in | std::ifstream::binary);

		// Check that file is open
		if (file.is_open() == false)
		{
			// Error
			throw map_file_exception("Can't open file '" + path + "'!");
		}

		// Read magic number
		file.read(reinterpret_cast<char*>(&m_magic), sizeof(m_magic));

		// Check the magic number
		if (m_magic != magic)
		{
			// Unknown file format!
			throw map_file_exception("Unknown map file format '" + path + "'!");
		}

		// Read header
		file.read(reinterpret_cast<char*>(&m_width), sizeof(m_width));
		file.read(reinterpret_cast<char*>(&m_height), sizeof(m_height));
		file.read(reinterpret_cast<char*>(&m_section_width), sizeof(m_section_width));
		file.read(reinterpret_cast<char*>(&m_section_height), sizeof(m_section_height));

		// Calculate section number
		const unsigned int section_num = (m_width / m_section_width)
			* (m_height / m_section_height);

		// Current offset
		unsigned int offset = 0;

		// Read sections
		try
		{
			// Reserve memory for sections
			m_sections.reserve(section_num);

			// Read sections
			for (unsigned int i = 0; i < section_num; ++i)
			{
				// Current map section
				section sec;

				// Read offset to section
				file.read(reinterpret_cast<char*>(&offset), sizeof(offset));

				// Check offset
				if (offset > 0)
				{
					// Current offset
					std::streampos cur = file.tellg();

					// Go to offset
					file.seekg(offset, file.beg);

					// Read section
					file.read(reinterpret_cast<char*>(sec.tiles), section_size);

					// Go back
					file.seekg(cur, file.beg);

					// Section is not empty
					sec.empty = false;
				}

				// Add section to the list
				m_sections.push_back(sec);
			}
		}
		catch (const std::bad_alloc&)
		{
			// Not enough memory
			throw map_file_exception("Not enough memory to load map file '" + path + "'!");
		}


		// Close file
		file.close();

	}
}