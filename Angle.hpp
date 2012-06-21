#ifndef GAME3D_ANGLE_HPP
#define GAME3D_ANGLE_HPP

#include <cmath>
#include "Float.hpp"

namespace Game3D {

	const Float PI = 3.1415926535898;

	class Angle {
		public:
			inline Angle()
				: degrees_(0.0){ }
			
			inline static Angle Degrees(Float degrees){
				while(degrees < -180.0){
					degrees += 360.0;
				}
			
				while(degrees > 180.0){
					degrees -= 360.0;
				}
			
				Angle angle;
				angle.degrees_ = degrees;
				return angle;
			}
			
			inline static Angle Radians(const Float& radians){
				return Angle::Degrees(radians / (PI / 180.0));
			}
			
			inline Angle operator+(const Angle& angle) const {
				return Angle::Degrees(degrees_ + angle.degrees_);
			}
			
			inline Angle operator-(const Angle& angle) const {
				return Angle::Degrees(degrees_ - angle.degrees_);
			}
			
			inline Float degrees() const{
				return degrees_;
			}
			
			inline Float radians() const{
				return degrees_ * (PI / 180.0);
			}
		
		private:
			Float degrees_;
		
	};
	
}

#endif
