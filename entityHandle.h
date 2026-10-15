#pragma once

#include <cstdint>
/*
Sometimes, components need to reference other components. Technically
this is not needed but building the app around this can potentially reduce number of bugs
(e.g. components holding stale entity references)
*/
struct EntityHandle {
	uint32_t index;
	uint32_t generation;
};