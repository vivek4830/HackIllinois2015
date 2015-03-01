camera: camera.cpp
	g++ camera.cpp -o camera `pkg-config --cflags --libs opencv`

evidence_finger_tracking: finger_tracker.h finger_tracker.cpp evidence_finger_tracker.cpp
	g++ evidence_finger_tracker.cpp finger_tracker.cpp finger_tracker.h -o evidence_finger_tracker `pkg-config --cflags --libs opencv`

2evidence: finger_tracker.h finger_tracker.cpp 2evidence_finger_tracker.cpp
	g++ 2evidence_finger_tracker.cpp finger_tracker.cpp finger_tracker.h -o 2evidence `pkg-config --cflags --libs opencv`

