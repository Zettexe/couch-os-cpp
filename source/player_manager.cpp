#include "musicos/player_manager.h"
#include <spdlog/spdlog.h>

std::unordered_map<dpp::snowflake, player_d> player_manager_c::players;

void player_d::stream(std::string file_path) {
  ogg_sync_state sync;
  ogg_stream_state stream_state;
  ogg_page page;
  ogg_packet packet;
  char *buffer;

  FILE *file_stream;
  file_stream = fopen(file_path.c_str(), "rb");

  fseek(file_stream, 0L, SEEK_END);
  size_t file_stream_position = ftell(file_stream);
  rewind(file_stream);

  ogg_sync_init(&sync);

  int end_of_stream = 0;
  int i;

  buffer = ogg_sync_buffer(&sync, file_stream_position);
  fread(buffer, 1, file_stream_position, file_stream);

  ogg_sync_wrote(&sync, file_stream_position);

  if (ogg_sync_pageout(&sync, &page) != 1) {
    // TODO: Does not appear to be an ogg stream
    spdlog::error("Not an ogg stream");
    return;
  }

  ogg_stream_init(&stream_state, ogg_page_serialno(&page));

  if (ogg_stream_pagein(&stream_state, &page) < 0) {
    // TODO: Error reading initial page of ogg stream
    spdlog::error("Cannot read inital page of ogg stream");
    return;
  }

  if (ogg_stream_packetout(&stream_state, &packet) != 1) {
    // TODO: Error reading header packet of ogg stream
    spdlog::error("Cannot read header packet of ogg stream");
    return;
  }

  while (true) {
    // {
    //   std::lock_guard<std::mutex> lk(this->sq_m);
    //   auto sq = vector_find(&this->stop_queue, server_id);
    //   if (sq != this->stop_queue.end())
    //     break;
    // }

    if (!voice_client || voice_client->terminating) {
      // TODO: Can't continue streaming, connection broken
      spdlog::error("Connection broken");
      break;
    }

    if (ogg_sync_pageout(&sync, &page) != 1) {
      // fprintf(stderr, "[ERROR
      // MANAGER::STREAM] Can't continue
      // streaming, corrupt audio (need
      // recapture or incomplete audio
      // file)\n");
      spdlog::error("Corrupt audio");
      break;
    }

    if (ogg_stream_pagein(&stream_state, &page) < 0) {
      // TODO: Can't continue streaming, error reading ogg page
      spdlog::error("Cant read ogg page");
      break;
    }

    /* Now loop though all the pages and send the packets to the vc */
    while (ogg_sync_pageout(&sync, &page) == 1) {
      ogg_stream_init(&stream_state, ogg_page_serialno(&page));

      if (ogg_stream_pagein(&stream_state, &page) < 0) {
        spdlog::error("Error reading page of Ogg bitstream data.");
        break;
      }

      while (ogg_stream_packetout(&stream_state, &packet) != 0) {
        /* Send the audio */
        // int samples = opus_packet_get_samples_per_frame(packet.packet, 48000);

        voice_client->send_audio_opus(packet.packet, packet.bytes /* , samples / 48 */);
      }
    }
  }

  /* Cleanup */
  ogg_stream_clear(&stream_state);
  ogg_sync_clear(&sync);
}