#ifndef GAME3D_MAP_HPP
#define GAME3D_MAP_HPP

namespace Game3D{

	template <typename T>
	class Array2D{
		public:
			Array2D() :
				width_(0), height_(0),
				data_(0){ }
				
			Array2D(std::size_t width, std::size_t height) :
				width_(width), height_(height),
				data_(new T[width * height]){ }
			
			T& at(std::size_t x, std::size_t y){
				assert(x < width_);
				assert(y < height_);
				return data_[width_ * y + x];
			}
			
			const T& at(std::size_t x, std::size_t y) const{
				assert(x < width_);
				assert(y < height_);
				return data_[width_ * y + x];
			}
			
			std::size_t width() const{
				return width_;
			}
			
			std::size_t height() const{
				return height_;
			}
		
		private:
			std::size_t width_, height_;
			T * data_;
		
	};
	
	enum ObjectType{
		WALL,
		
	};

	class Floor{
		public:
			Floor(std::size_t width, std::size_t height);
			
			bool 
			
		private:
			
		
	};
	
	struct Map{
		std::string name;
		std::vector<Floor> floors;
	};

}

#endif
