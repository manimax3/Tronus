#pragma once
#include <tr.h>


namespace tr
{
	class TR_API Vec2
	{
	public:
		union {
			float m_Values[2];
			struct {
				float x, y;
			};
		};
	};
}