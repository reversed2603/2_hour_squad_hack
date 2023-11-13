#pragma once

class vec2 {
public:
	float x, y;

	inline vec2( ) {
		x = y = 0.0f;
	}

	inline vec2 operator/( float v ) const {
		return vec2( x / v, y / v );
	}
	inline vec2( float X, float Y ) {
		x = X; y = Y;
	}

	inline vec2 operator-( const vec2& v ) const {
		return vec2( x - v.x, y - v.y );
	}

	inline vec2 operator+( const vec2& v ) const {
		return vec2( x + v.x, y + v.y );
	}
	
	inline vec2& operator*=( const vec2& v ) {
		x *= v.x; y *= v.y; return *this;
	}

	inline vec2& operator+=( const vec2& v ) {
		x += v.x; y += v.y; return *this;
	}

	inline vec2& operator-=( const vec2& v ) {
		x -= v.x; y -= v.y; return *this;
	}

	inline vec2 operator*( const float& v ) const {
		return vec2( x * v, y * v );
	}

	inline vec2& operator*=( float v ) {
		x *= v; 
		y *= v; 
		return *this;
	}

	inline bool is_zero( ) const {
		return ( x > -0.1f && x < 0.1f && y > -0.1f && y < 0.1f );
	}

	inline vec2& operator/=( float fl )
	{
		x /= fl;
		y /= fl;
		return *this;
	}

	inline bool operator==( const vec2& v ) const {
		if( this->x == v.x && this->y == v.y )
			return true;
		else
			return false;
	}

	inline vec2 normalize( )
	{
		if( x > 180 )
			x -= 360;
		else if( x < -180 )
			x += 360;

		if( y > 180 )
			y -= 360;
		else if( y < -180 )
			y += 360;

		if( x < -89 )
			x = -89;

		if( x > 89 )
			x = 89;

		while( y < -180.0f )
			y += 360.0f;

		while( y > 180.0f )
			y -= 360.0f;

		return *this;
	}

	inline bool bigger( float x, float y, bool check_equals = false ) const
	{
		return check_equals ? ( this->x >= x || this->y >= y )
			: ( this->x > x || this->y > y );
	}

	inline float length( ) const
	{
		return std::sqrt( 
			( x * x )
			+
			( y * y )
		 );
	}

	vec2 normalized( ) const
	{
		return vec2( x / length( ), y / length( ) );
	}

	float distance( vec2 input ) const
	{
		return ( *this - input ).length( );
	}
};

class vec3
{
public:
	float x, y, z;

	inline vec3( ) {
		x = y = z = 0.0f;
	}

	inline vec3( float X, float Y, float Z ) {
		x = X; y = Y; z = Z;
	}

	inline float operator[ ]( int i ) const {
		return ( ( float* )this )[ i ];
	}

	inline vec3& operator-=( float v ) {
		x -= v; y -= v; z -= v; return *this;
	}

	inline vec3 operator-( const vec2& v ) const {
		return vec3( x - v.x, y - v.y, z );
	}

	inline vec3 operator*( float v ) const {
		return vec3( x * v, y * v, z * v );
	}

	inline vec3 operator/( float v ) const
	{
		return vec3( x / v, y / v, z / v );
	}

	inline vec3& operator+=( const vec3& v ) {
		x += v.x; y += v.y; z += v.z; return *this;
	}

	inline vec3& operator+=( const float& v ) {
		x += v; y += v; z += v; return *this;
	}

	inline vec3& operator*=( const float& v ) {
		x *= v; y *= v; z *= v; return *this;
	}

	inline vec3& operator*=( const vec3& v ) {
		x *= v.x; y *= v.y; z *= v.z; return *this;
	}

	inline vec3& operator-=( const vec3& v ) {
		x -= v.x; y -= v.y; z -= v.z; return *this;
	}

	inline vec3 operator-( const vec3& v ) const {
		return vec3( x - v.x, y - v.y, z - v.z );
	}

	inline vec3 operator+( const vec3& v ) const {
		return vec3( x + v.x, y + v.y, z + v.z );
	}

	inline float length( )
	{
		auto number = ( x * x + y * y + z * z );
		long        i;
		float       x2, y;
		const float threehalfs = 1.5F;

		x2 = number * 0.5F;
		y = number;
		i = *( long* )&y; // floating point bit level hacking [ sic ]
		i = 0x5f3759df - ( i >> 1 ); // Newton's approximation
		y = *( float* )&i;
		y = y * ( threehalfs - ( x2 * y * y ) ); // 1st iteration
		y = y * ( threehalfs - ( x2 * y * y ) ); // 2nd iteration

		return 1 / y;
	}

	inline vec3 normalize( )
	{
		float mag = length( );
		if( mag > 0.00001f )
		{
			x = x / mag;
			y = y / mag;
			z = z / mag;
		}
		else
		{
			x = 0;
			y = 0;
			z = 0;
		}
		return *this;
	}

	inline bool is_zero( )
	{
		return this->x == 0 && this->y == 0 && this->z == 0;
	}

	inline float length2d( vec3 apple )
	{
		return sqrtf( 
			( apple.x * apple.x )
			+
			( apple.z * apple.z ) );
	}

	inline bool operator==( const vec3& v ) const {
		return this->x == v.x && this->y == v.y && this->z == v.z;
	}

	inline bool operator==( const float v ) const {
		return this->x == v && this->y == v && this->z == v;
	}

	inline bool operator!=( const vec3& v ) const {
		return this->x == v.x && this->y == v.y && this->z == v.z;
	}

	inline bool operator!=( const float v ) const {
		return this->x == v && this->y == v && this->z == v;
	}

	float dot_product( vec3 input ) const {
		return ( x * input.x ) + ( y * input.y ) + ( z * input.z );
	}

	float unity_length( )
	{
		return sqrt( ( x * x + y * y + z * z ) );
	}

	vec3 cross( vec3 rhs )
	{
		return vec3 ( y * rhs.z - z * rhs.y, z * rhs.x - x * rhs.z, x * rhs.y - y * rhs.x );
	}

	void clear( )
	{
		this->x = this->y = this->z = 0;
	}

	vec3 unity_normalize ( )
	{
		float num = unity_length( );
		if( num > 1E-05f )
		{
			x /= num;
			y /= num;
			z /= num;
		}
		else
		{
			x = 0;
			y = 0;
			z = 0;
		}

		return vec3( x, y, z );
	}

	float dot( const vec3& vector )
	{
		return x * vector.x + y * vector.y + z * vector.z;
	}

	float distance ( vec3 input ) const
	{
		return ( *this - input ).length ( );
	}
};

struct vec4
{
	float x, y, z, w;
};
