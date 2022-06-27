#pragma once

namespace FlatEngine {
	class Core {
	public:
		enum EngineState {
			EDITING,
			PLAYING,
			PAUSED,
		};
		static void Init();
		static void SetEngineState(EngineState state);
		static EngineState GetEngineState();
	};
}
