

if (!APPLE)
    message(FATAL "platform_apple.cmake must be included only on Apple platform")
endif (!APPLE)

link_directories(/Library/Frameworks/GDAL.framework/unix/lib/)
