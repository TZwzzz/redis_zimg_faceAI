# redis_zimg_faceAI
这是sensor捕捉图片后上传zimg图片服务器，云服务器会下发一个redis stream格式的消息，本程序取该消息中的url字段以及算法字段，对url中的图片使用对应算法进行检测，并将检测结果以redis stream的格式发送
需要的依赖项：
opencv-3.4.12 + opencv_controb-3.4.12 c++版本
hiredis
spdlog
nlohmann-json3-dev
libcurl4-openssl-dev
人脸检测使用到的模型文件：
opencv_face_detector_uint8.pb
opencv_face_detector_pbtxt
