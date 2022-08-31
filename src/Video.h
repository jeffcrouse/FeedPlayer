#pragma once
#include <vlc/vlc.h>
#include <mutex>
#include "Texture.h"

namespace feed {
	class Video {
	public:
		Video();
		~Video();
		Video(const Video&) { Video::init(); }		// Copy constructor
		Video& operator=(const Video&) = delete;	// delete = operator

		void load(std::string path);
		void play();
		void stop();
		void update(Texture& tex);
		bool is_playing();
		void set_pause(bool);
		inline void toggle_pause() { set_pause(is_playing()); }


		inline unsigned int getWidth() const { return width; }
		inline unsigned int getHeight() const { return height; }

	protected:

		unsigned int width;
		unsigned int height;

		std::mutex mutex;
		unsigned char* pixelBuffer;
		bool newPixels = false;
		libvlc_media_player_t* mp;

		static void init();
		static libvlc_instance_t* inst;
	};

}