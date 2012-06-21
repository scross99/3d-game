#ifndef GAME3D_FLOAT_HPP
#define GAME3D_FLOAT_HPP

#include <cmath>

namespace Game3D {

	class Float {
		public:
			inline Float()
				: value_(0.0) { }
				
			inline Float(double v)
				: value_(v) { }
				
			inline double value() const {
				return value_;
			}
			
			inline Float abs() const {
				return Float(fabs(value_));
			}
			
			inline int sign() const {
				return value_ > 0.0 ? 1 : (value_ < 0.0 ? -1 : 0);
			}
			
			inline Float posVal() const{
				return value_ > 0.0 ? *this : Float(0.0);
			}
			
			inline Float sqrRoot() const {
				return Float(sqrt(abs().value()));
			}
			
			inline Float operator+() const{
				return *this;
			}
			
			inline Float operator-() const{
				return Float(-value_);
			}
			
			inline Float operator+(const Float& value) const {
				return Float(value_ + value.value_);
			}
			
			inline Float operator-(const Float& value) const {
				return Float(value_ - value.value_);
			}
			
			inline Float operator*(const Float& value) const {
				return Float(value_ * value.value_);
			}
			
			inline Float operator/(const Float& value) const {
				return Float(value_ / value.value_);
			}
			
			inline void operator+=(const Float& value) {
				value_ += value.value_;
			}
			
			inline void operator-=(const Float& value) {
				value_ -= value.value_;
			}
			
			inline void operator*=(const Float& value) {
				value_ *= value.value_;
			}
			
			inline void operator/=(const Float& value) {
				value_ /= value.value_;
			}
			
			inline bool operator==(const Float& value) const {
				return value_ == value.value_;
			}
			
			inline bool operator!=(const Float& value) const {
				return value_ != value.value_;
			}
			
			inline bool operator>(const Float& value) const {
				return value_ > value.value_;
			}
			
			inline bool operator<(const Float& value) const {
				return value_ < value.value_;
			}
			
			inline bool operator>=(const Float& value) const {
				return value_ >= value.value_;
			}
			
			inline bool operator<=(const Float& value) const {
				return value_ <= value.value_;
			}
			
		private:
			double value_;
	};
	
}

#endif
