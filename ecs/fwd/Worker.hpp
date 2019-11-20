#pragma once

namespace ecs {
	template<typename TaskType, template<typename ...> class ContainerType = std::deque>
	struct Worker;
} // ecs