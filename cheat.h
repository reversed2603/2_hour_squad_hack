#pragma once
#include <cstdint>
#include <misc/offsets.h>
#include <misc/math.h>
#include <misc/memory.h>
#include <misc/render.h>
#include <iostream>

bool esp = true;
bool draw_lines = true;
bool draw_skeleton = true;
bool draw_box = true;
bool aimbot_enabled = true;
bool draw_teammates = true;
bool kill_all = false;
bool aim_target = false;
bool show_aim_fov = false;
bool aim_at_nearest_bone = false;
float aim_fov = 65.f;
int aim_hitbox = 0;

int smoothness_amount;

static std::string name_by_id( uint32_t actor_id ) {

	auto chunk_offset = ( UINT )( ( int )( actor_id ) >> 16 );

	auto name_offset = ( USHORT )actor_id;

	auto name_pool_chunk = *reinterpret_cast< UINT64* >( mem_data::base_handle + mem_data::offsets::g_name + ( ( chunk_offset + 2 ) * 8 ) );
	auto entry_offset = name_pool_chunk + ( ULONG )( 2 * name_offset );
	auto name_entry = *reinterpret_cast< INT16* >( entry_offset );

	auto name_length = name_entry >> 6;
	char buff[1028];
	if ( name_length )
	{
		memcpy( buff, ( LPCVOID )( entry_offset + 2 ), name_length );
		buff[name_length] = '\0';
		return std::string( buff );
	}
	else 
		return "";
}

int aimbot_bone = 0;
inline std::vector< bones > scanning_bones = {
	HEAD, NECK, CHEST, PELVIS,
	RIGHT_SHOULDER, RIGHT_ELBOW, RIGHT_HAND,
	LEFT_SHOULDER, LEFT_ELBOW, LEFT_HAND,
	RIGHT_THIGH, RIGHT_KNEE, RIGHT_FOOT,
	LEFT_THIGH, LEFT_KNEE, LEFT_FOOT,
};

void get_best_bone( uintptr_t mesh, vec2 center ) {
    if( !aim_at_nearest_bone ) {
		switch( aim_hitbox ) {
			case 0:
			{
				aimbot_bone = HEAD;
				break;
			}
			case 1:
			{
				aimbot_bone = NECK;
				break;
			}
			case 2:
			{
				aimbot_bone = PELVIS;
				break;
			}
			case 3:
			{
				aimbot_bone = STOMACH;
				break;
			}
		}
	}
    else {
        float closest = FLT_MAX;
        aimbot_bone = -1; // reset this too

        for( int i = 0; i < scanning_bones.size( ); i++ ) {
            vec3 pos3d = get_bone_location( mesh, scanning_bones[ i ] );

            vec2 pos = vec2( );
			w2s( pos3d, &pos, false );
            if( pos.is_zero( ) )
                continue;

            float length = center.distance( pos );

            if( length < closest
                && length < aim_fov ) {
                closest = length;
                aimbot_bone = scanning_bones[ i ];
            }
        }
    }
}

bones translate_from_slider( int idx ) {
	switch ( idx ) {
	case 0:
		return bones::HEAD;
		break;
	case 1:
		return bones::CHEST;
		break;
	case 2:
		return bones::PELVIS;
		break;
	}
}

inline vec3 calculate_angles( vec3 target_location, vec3 my_location )
{
	vec3 VectorPos = vec3( target_location.x - my_location.x, target_location.y - my_location.y, target_location.z - my_location.z );

	double distance = ( double )( sqrt( VectorPos.x * VectorPos.x + VectorPos.y * VectorPos.y + VectorPos.z * VectorPos.z ) );

	vec3 rot;
	rot.x = -( ( acos( VectorPos.z / distance ) * ( double )( 180 / 3.14f ) ) - 90 );
	rot.y = atan2( VectorPos.y, VectorPos.x ) * ( double )( 180 / 3.14f );

	return rot;
}

vec3 location = vec3( ), rotation = vec3( );

uintptr_t target_pawn = NULL;
float closest_to_center = 0;

void render_skeleton( uintptr_t mesh, ImU32 color )
{
	std::vector< std::vector< bones > > scan_bones =
	{
		{ bones::HEAD, bones::NECK, bones::CHEST, bones::PELVIS },
		{ bones::NECK, bones::RIGHT_SHOULDER, bones::RIGHT_ELBOW, bones::RIGHT_HAND},
		{ bones::NECK, bones::LEFT_SHOULDER, bones::LEFT_ELBOW, bones::LEFT_HAND },
		{ bones::PELVIS, bones::RIGHT_THIGH, bones::RIGHT_KNEE, bones::RIGHT_FOOT },
		{ bones::PELVIS, bones::LEFT_THIGH, bones::LEFT_KNEE, bones::LEFT_FOOT},
	};

	for ( const std::vector< bones >& this_bone : scan_bones )
	{
		vec2 previous{};
		for ( const bones& bone : this_bone )
		{
			vec2 bone_pos = {};
			if( !w2s( get_bone_location( mesh, bone ), &bone_pos, false ) )
				continue;

			if ( bone_pos.is_zero( ) )
				break;

			if( previous.is_zero( ) ) // first bone no draw happens
			{
				previous = bone_pos;
				continue;
			}

			draw_line( ImVec2( previous.x, previous.y ), ImVec2( bone_pos.x, bone_pos.y ), color, 0.75f );
			previous = bone_pos;
		}
	}
}

void smooth_angles( vec2& angles, vec2 local_view_angles ) {
	float smoothness = smoothness_amount;
    if( smoothness ) {
		vec2 delta = vec2( angles - local_view_angles ).normalize( );

		float smooth[ 3 ]{ }; // 0 is regular yaw, and 1 is pitch. 2 is being used for constant smooth.

		float smoothing_value = 5.5f * ( smoothness / 100.f );
		float max_smooth = powf( 0.81f + smoothing_value, 0.4f );
		smooth[ 2 ] = min( 0.98f, max_smooth );

		vec2 new_delta = vec2( );

		float coeff = ( 1.0f - smooth[ 2 ] ) / delta.length( ) * 4.f;

		// fast end
		coeff = powf( coeff, 2.f ) * 80.f / smoothness;

		coeff = min( 1.f, coeff );
		new_delta = delta * coeff;

		angles = local_view_angles + new_delta;
	}
}

void start_loop( ) {
	mem_data::base_handle = reinterpret_cast< uintptr_t >( GetModuleHandle( NULL ) );

	if ( esp ) {
		mem_data::pointers::g_world = *reinterpret_cast< uintptr_t* >( mem_data::base_handle + mem_data::offsets::g_world );
		if( !mem_data::pointers::g_world )
			return;

		mem_data::pointers::game_instance = *reinterpret_cast< uintptr_t* >( mem_data::pointers::g_world + mem_data::offsets::game_instance );
		if( !mem_data::pointers::game_instance )
			return;

		mem_data::pointers::local_player_array = *reinterpret_cast< uintptr_t* >( mem_data::pointers::game_instance + mem_data::offsets::local_players_array );
		if( !mem_data::pointers::local_player_array )
			return;

		mem_data::pointers::local_player = *reinterpret_cast< uintptr_t* >( mem_data::pointers::local_player_array );		
		if( !mem_data::pointers::local_player ) 
			return;

		mem_data::pointers::local_player_controller = *reinterpret_cast< uintptr_t* >( mem_data::pointers::local_player + mem_data::offsets::player_controller );	
		if( !mem_data::pointers::local_player_controller )
			return;

		mem_data::pointers::local_player_camera = *reinterpret_cast< uintptr_t* >( mem_data::pointers::local_player_controller + mem_data::offsets::camera_manager );	
		if( !mem_data::pointers::local_player_camera )
			return;

		mem_data::pointers::local_player_pawn = *reinterpret_cast< uintptr_t* >( mem_data::pointers::local_player_controller + mem_data::offsets::apawn );	
		if( !mem_data::pointers::local_player_pawn )
			return;

		mem_data::pointers::local_player_root = *reinterpret_cast< uintptr_t* >( mem_data::pointers::local_player_pawn + mem_data::offsets::root_component );	
		if( !mem_data::pointers::local_player_root )
			return;

		mem_data::pointers::local_player_state = *reinterpret_cast< uintptr_t* >( mem_data::pointers::local_player_pawn + mem_data::offsets::player_state );	
		if( !mem_data::pointers::local_player_state )
			return;

		mem_data::pointers::persistent_level = *reinterpret_cast< uintptr_t* >( mem_data::pointers::g_world + mem_data::offsets::persistent_level );	
		if( !mem_data::pointers::persistent_level )
			return;

		mem_data::pointers::actors = *reinterpret_cast< uintptr_t* >( mem_data::pointers::persistent_level + mem_data::offsets::actor_array );	
		if( !mem_data::pointers::actors )
			return;

		int actor_count = *reinterpret_cast< int* >( mem_data::pointers::persistent_level + mem_data::offsets::actor_count );

		int32_t local_player_teamid = *reinterpret_cast< int32_t* >( mem_data::pointers::local_player_state + mem_data::offsets::player_teamid );

		vec3 local_player_loc = *reinterpret_cast< vec3* >( mem_data::pointers::local_player_root + mem_data::offsets::relative_location );

		location = *reinterpret_cast< vec3* >( mem_data::pointers::local_player_camera + 0x1B00 ); // Vector3 GetCameraLocation( )
		rotation = *reinterpret_cast< vec3* >( mem_data::pointers::local_player_camera + 0x1B00 + 0xC ); // FRotator GetCameraRotation( )

		int32_t width, height;
		get_screen_size( mem_data::pointers::local_player_controller, width, height );

		closest_to_center = target_pawn = NULL;

		for ( int i = 0; i < actor_count; i++ )
		{
			uintptr_t actor_pawn = *reinterpret_cast< uintptr_t* >( mem_data::pointers::actors + i * sizeof( uint64_t ) );
			if ( !actor_pawn 
				|| actor_pawn == mem_data::pointers::local_player_pawn )
				continue;

			uintptr_t uinstigator = *reinterpret_cast< uintptr_t* >( actor_pawn + mem_data::offsets::instigator_actor );
			if( !uinstigator 
				|| uinstigator == mem_data::pointers::local_player_pawn )
				continue;

			uintptr_t root = *reinterpret_cast< uintptr_t* >( uinstigator + mem_data::offsets::root_component );

			vec3 loc = *reinterpret_cast< vec3* >( root + mem_data::offsets::relative_location );
			if( loc.is_zero( ) )
				continue;

			uintptr_t state = *reinterpret_cast< uintptr_t* >( uinstigator + mem_data::offsets::player_state );
			uintptr_t mesh = *reinterpret_cast< uintptr_t* >( actor_pawn + mem_data::offsets::actor_mesh );

			int32_t teamid = *reinterpret_cast< int32_t* >( state + mem_data::offsets::player_teamid );

			float health = *reinterpret_cast< float* >( actor_pawn + mem_data::offsets::actor_health );

			if( health < 1 )
				continue;

			std::string name = name_by_id( *reinterpret_cast< int32_t* >( actor_pawn + mem_data::offsets::actor_id ) );

			bool is_team = teamid == local_player_teamid;

			if ( is_team
				&& !draw_teammates )
				continue;

			if ( name.substr( 0, 10 ) == _( "BP_Soldier" ) )
			{
				vec2 player_position{ };

				vec2 bottom_w2s = vec2( );
				if( !w2s( get_bone_location( mesh, ZERO ), &bottom_w2s, false ) )
					continue;

				vec3 head_pos = get_bone_location( mesh, HEAD );
				head_pos.z += 20;

				vec2 head_w2s = vec2( );
				if( !w2s( head_pos, &head_w2s, false ) )
					continue;

				float player_height = ( head_w2s.y - bottom_w2s.y );
				if( player_height < 0.f ) 
					player_height = player_height * -1.f;

				int player_width = player_height * 0.45;

				if( w2s( loc, &player_position, false ) ) {
					float dx = player_position.x - ( width / 2.f );
					float dy = player_position.y - ( height / 2.f );
					float dist = sqrt( dx * dx + dy * dy );

					if( aimbot_enabled
						&& !target_pawn ) {
						if ( dist < aim_fov
							|| dist < closest_to_center ) {

							closest_to_center = dist;
							target_pawn = actor_pawn;
						}
					}

					if( actor_pawn == target_pawn
						&& aim_target
						|| !aim_target ) {
						if( GetAsyncKeyState( VK_F1 )
							&& kill_all ) {
							*reinterpret_cast< vec3* >( root + mem_data::offsets::relative_location ) = local_player_loc;
						}
					}

					if( draw_box ) {
						draw_regular_box( ImColor( 255, 255, 255 ),
							head_w2s.x - ( player_width / 2.f ), head_w2s.y, 
							player_width, player_height );
					}

					if( draw_skeleton ) {
						render_skeleton( mesh, ImColor( 255, 255, 255 ) );
					}

					if( draw_lines ) {
						draw_line( ImVec2( width / 2, height ), ImVec2( bottom_w2s.x, bottom_w2s.y ), 
							( target_pawn && actor_pawn == target_pawn ) ? ImColor( 255, 0, 0 ) : is_team ? ImColor( 0, 255, 50 ) : ImColor( 255, 255, 255 ), 0.25f );
					}
				}
			}
		}

		if( aimbot_enabled ) {
			if( show_aim_fov )
				ImGui::GetBackgroundDrawList( )->AddCircle( ImVec2( width / 2, height / 2 ), aim_fov, ImColor( 0, 0, 0 ), 60, 1.f );

			if( target_pawn ) {
				uintptr_t actor = *reinterpret_cast< uintptr_t* >( target_pawn + mem_data::offsets::instigator_actor );
				if( !actor )
					return;

				uintptr_t root = *reinterpret_cast< uintptr_t* >( actor + mem_data::offsets::root_component );
				if( !root )
					return;

				uintptr_t mesh = *reinterpret_cast< uintptr_t* >( target_pawn + mem_data::offsets::actor_mesh );
				if( !mesh )
					return;	

				get_best_bone( mesh, vec2( width / 2, height / 2 ) );

				vec3 aim_angle = ( calculate_angles( get_bone_location( mesh, translate_from_slider( aim_hitbox ) ), location ) ) - rotation;

				uintptr_t connect_message = *reinterpret_cast< uintptr_t* >( mem_data::pointers::local_player_controller + 0x420 );

				vec3 controller_rotation = *reinterpret_cast< vec3* >( mem_data::pointers::local_player_controller + 0x420 + 0x8 );

				vec2 aim_xy = vec2( aim_angle.x, aim_angle.y );

				smooth_angles( aim_xy, vec2( controller_rotation.x, controller_rotation.y ) );

				if( GetAsyncKeyState( VK_RBUTTON ) )
					*reinterpret_cast< vec3* >( mem_data::pointers::local_player_controller + 0x420 + 0x8 ) = vec3( aim_xy.x / 1.2f, aim_xy.y / 1.2f, aim_angle.z / 1.2 );
			}
		}
	}
}

