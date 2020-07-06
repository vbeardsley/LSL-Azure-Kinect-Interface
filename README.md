# LSL-Azure-Kinect-Interface

To get the project to run, you also need to add the file dnn_model_2_0.onnx to the KinectLSL/Azure Kinect Body Tracking SDK/sdk/windows-desktop/amd64/release/bin folder.
You also need to add both that same file and the file cudnn64_7.dll to the KinectLSL/Azure Kinect Body Tracking SDK/tools folder.

Note: The four files titled "Receive[type].py" are not automatically run when the project is run. For best results, start up the receive files first, and then run the code found in Reader.cpp.
