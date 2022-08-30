#include "Video.h"
#include <iostream>
#include <sstream>
#include <filesystem>

using namespace feed;

libvlc_instance_t* Video::inst = NULL;


// --------------------------------------------------------------------------------------
Video::Video() {
	Video::init();
}

// --------------------------------------------------------------------------------------
Video::~Video() {
	libvlc_media_player_release(mp);
	libvlc_release(Video::inst);
}

// --------------------------------------------------------------------------------------
void Video::init() {
	if (Video::inst == NULL) {
		const char* argv[] = { "--no-xlib", "--no-video-title-show" };
		//int argc = sizeof(argv) / sizeof(*argv);
		Video::inst = libvlc_new(2, argv);
		if (inst == NULL) {
			throw std::runtime_error("[VLC error] Couldn't make instance.");
		}
		else {
			std::cout << "libvlc inited" << std::endl;
		}
	} else {
		libvlc_retain(Video::inst);
	}
}

// --------------------------------------------------------------------------------------
void Video::load(std::string path)
{
	if (!std::filesystem::exists(path)) {
		throw std::runtime_error(path + " does not exist");
	}

	libvlc_media_t* m;
	m = libvlc_media_new_path(inst, path.c_str());
	if (m == NULL) {
		const char* err = libvlc_errmsg();
		throw std::runtime_error("couldn't open " + path + " - " + std::string(err));
	}
	else {
		std::cout << "loaded " << path << std::endl;
	}

	libvlc_media_track_info_t* info;
	libvlc_media_parse(m);
	int i = libvlc_media_get_tracks_info(m, &info);
	for (--i; i >= 0; --i) {
		if (info[i].i_type == libvlc_track_video) {
			width = info[i].u.video.i_width;
			height = info[i].u.video.i_height;
		}
	}

	mp = libvlc_media_player_new_from_media(m);
	libvlc_media_release(m);

	//libvlc_video_get_size(mp, 0, &width, &height);

	std::cout << "width = " << width << " height = " << height << std::endl;
	pixelBuffer = new unsigned char[width * height * 3];


	auto video_lock = [](void* data, void** p_pixels) -> void* {
		Video* me = (Video*)data;
		me->mutex.lock();
		p_pixels[0] = (void*)me->pixelBuffer;
		return NULL;
	};

	auto video_unlock = [](void* data, void* id, void* const* p_pixels) {
		Video* me = (Video*)data;
		me->newPixels = true;
		me->mutex.unlock();
	};

	auto video_display = [](void* opaque, void* picture) {};

	libvlc_video_set_callbacks(mp, video_lock, video_unlock, video_display, this);
	libvlc_video_set_format(mp, "RV24", width, height, width * 3);
}

// --------------------------------------------------------------------------------------
void Video::play()
{
	libvlc_media_player_play(mp);
}

// --------------------------------------------------------------------------------------
void Video::stop()
{
	libvlc_media_player_stop(mp);
}

// --------------------------------------------------------------------------------------
void Video::set_pause(bool pause)
{
	libvlc_media_player_set_pause(mp, pause);
}


// --------------------------------------------------------------------------------------
bool Video::is_playing()
{
	return libvlc_media_player_is_playing(mp);
}



// --------------------------------------------------------------------------------------
void Video::update(Texture& tex)
{
	if (mutex.try_lock()) {
		if (newPixels) {
			tex.set(pixelBuffer, width, height);
			newPixels = false;
		}
		mutex.unlock();
	}
}