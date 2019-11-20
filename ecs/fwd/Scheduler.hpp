#pragma once

namespace ecs {
    template<typename WorkerType = ecs::impl::Worker<ecs::Job, std::deque>>
	class PrimitiveScheduler;
} // ecs