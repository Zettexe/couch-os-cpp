#include "musicos/player_manager.h"
#include "wiz/cache.h"
#include <chrono>
#include <dpp/dispatcher.h>
#include <oggz/oggz.h>
#include <spdlog/spdlog.h>
#include <thread>

void player_d::stream(std::string file_name) {
  spdlog::debug("Start Streaming: {}", file_name);
  std::string file_path = "music/" + file_name + ".opus";
  OGGZ *track_og = oggz_open(file_path.c_str(), OGGZ_READ);

  if (track_og) {
    oggz_set_read_callback(
      track_og, -1,
      [](OGGZ *oggz [[gnu::unused]], oggz_packet *packet, long serialno [[gnu::unused]],
         void *user_data) {
        ((dpp::discord_voice_client *)user_data)
          ->send_audio_opus(packet->op.packet, packet->op.bytes);
        return 0;
      },
      (void *)voice_client);

    // No need to worry about getting stuck in this loop
    // since the object will delete itself when disconnected
    while (true) {
      static const long READ_CHUNK = 16384;
      const long read_bytes = oggz_read(track_og, READ_CHUNK);

      if (!read_bytes) {
        voice_client->insert_marker("end_of_stream");
        break;
      }
    }
  }
}