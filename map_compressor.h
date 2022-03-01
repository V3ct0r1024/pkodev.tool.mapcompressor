#pragma once
#include <string>
#include <vector>
#include <stdexcept>


namespace pkodev
{
	// Point structure
	struct point
	{
		// Coordinates
		unsigned int x;
		unsigned int y;

		// Constructor
		point() :
			x(0),
			y(0)
		{

		}

		// Constructor
		point(unsigned int x_, unsigned int y_) :
			x(x_),
			y(y_)
		{

		}
	};

	// Exception for .map file
	class map_file_exception final : public std::runtime_error
	{
		public:

			// Constructor
			map_file_exception(const std::string& what) :
				std::runtime_error(what) { }

	};

	// .map file compressor class
	class map_compressor final
	{
		public:

			// Constructor
			map_compressor();

			// Copy constructor
			map_compressor(const map_compressor&) = delete;

			// Move constructor
			map_compressor(map_compressor&&) = delete;

			// Destructor
			~map_compressor();

			// Copy assigment operator
			map_compressor& operator=(const map_compressor&) = delete;

			// Move assigment operator
			map_compressor& operator=(map_compressor&&) = delete;

			// Open .map file
			void open(const std::string& path);

			// Compress .map file
			void compress(const std::string& path);

			// Get map width
			inline unsigned int width() const { return m_width; }

			// Get map height
			inline unsigned int height() const { return m_height; }

		private:

			// Some useful constants
			static const unsigned int magic{ 0x000BE953 };
			static const unsigned int tile_size{ 15 };
			static const unsigned int max_section_width{ 8 };
			static const unsigned int max_section_height{ 8 };
			static const unsigned int section_size{ max_section_width * max_section_height * tile_size };

			// Map section structure
			struct section
			{
				// Tiles data
				unsigned char tiles[section_size];

				// A flag that the section has no tiles
				bool empty;

				// Constructor
				section() :
					empty(true)
				{
					std::memset(reinterpret_cast<void*>(tiles), 0x00, section_size);
				}
			};

			// Get a section by coordinates X, Y
			section& get_section_xy(const point& pos);


			// Magic number
			unsigned int m_magic;

			// Map width
			unsigned int m_width;

			// Map height
			unsigned int m_height;

			// Map section width
			unsigned int m_section_width;

			// Map section height
			unsigned int m_section_height;

			// Map sections list
			std::vector<section> m_sections;



	};


}
