#pragma once

ImDrawList* draw_list = ImGui::GetOverlayDrawList( );

void draw_line( const ImVec2& aPoint1, const ImVec2 aPoint2, ImU32 aColor, const FLOAT aLineWidth )
{
	draw_list->AddLine( aPoint1, aPoint2, aColor, aLineWidth );
}

void draw_regular_box( ImColor color, float x, float y, float w, float h )
{
	draw_line( ImVec2( x, y ), ImVec2( x + w, y ), color, 1.f ); // top 
	draw_line( ImVec2( x, y - 1.f ), ImVec2( x, y + h + 1.f ), color, 1.f ); // left
	draw_line( ImVec2( x + w, y - 1.f ), ImVec2( x + w, y + h + 1.f ), color, 1.f );  // right
	draw_line( ImVec2( x, y + h ), ImVec2( x + w, y + h ), color, 1.f );   // bottom 
}