#include "musicos/player_manager.h"
#include "wiz/cache.h"
#include <chrono>
#include <dpp/dispatcher.h>
#include <opus/opus.h>
#include <spdlog/spdlog.h>
#include <thread>

void player_d::stream(std::string file_name) {
  // voice_client->set_send_audio_type(dpp::discord_voice_client::satype_overlap_audio);
  ogg_sync_state sync;
  ogg_stream_state stream_state;
  ogg_page page;
  ogg_packet packet;
  char *buffer;

  FILE *file_stream;
  std::string file_path = "music/" + file_name;
  file_stream = fopen(file_path.c_str(), "rb");

  fseek(file_stream, 0L, SEEK_END);
  size_t file_stream_position = ftell(file_stream);
  rewind(file_stream);

  ogg_sync_init(&sync);

  buffer = ogg_sync_buffer(&sync, file_stream_position);
  fread(buffer, 1, file_stream_position, file_stream);
  fclose(file_stream);
  file_stream = NULL;

  ogg_sync_wrote(&sync, file_stream_position);

  if (ogg_sync_pageout(&sync, &page) != 1) {
    spdlog::error("Not an ogg stream");
    return;
  }

  ogg_stream_init(&stream_state, ogg_page_serialno(&page));

  if (ogg_stream_pagein(&stream_state, &page) < 0) {
    spdlog::error("Error reading inital page of ogg stream");
    return;
  }

  if (ogg_stream_packetout(&stream_state, &packet) != 1) {
    spdlog::error("Error reading header packet of ogg stream");
    return;
  }

  if (!voice_client || voice_client->terminating) {
    spdlog::error("Connection broken");
    return;
  }

  if (ogg_sync_pageout(&sync, &page) != 1) {
    spdlog::error("Corrupt audio");
    return;
  }

  if (ogg_stream_pagein(&stream_state, &page) < 0) {
    spdlog::error("Error reading ogg page");
    return;
  }

  // voice_client->insert_marker(file_name);

  // Loop though all the pages and send the packets to the vc
  while (int page_out_result = ogg_sync_pageout(&sync, &page)) {
    if (page_out_result == -1) {
      spdlog::error("Invalid page format.");
      break;
    }
    if (page_out_result == 0) {
      spdlog::info("End of stream.");
      break;
    }

    ogg_stream_init(&stream_state, ogg_page_serialno(&page));

    if (ogg_stream_pagein(&stream_state, &page) < 0) {
      spdlog::error("Error reading page of Ogg bitstream data.");
      break;
    }

    while (int packet_out_result = ogg_stream_packetout(&stream_state, &packet)) {
      // packet_out_result = ogg_stream_packetout(&stream_state, &packet);
      if (packet_out_result == -1) {
        spdlog::warn("Missing or dropped packet.");
      }
      if (packet_out_result == 0) {
        spdlog::info("End of packet.");
        break;
      }

      voice_client->send_audio_opus(packet.packet, packet.bytes);
    }
  }

  spdlog::info("Cleanup");

  /* Cleanup */
  ogg_stream_clear(&stream_state);
  ogg_sync_clear(&sync);

  voice_client->insert_marker("end_of_stream");
}
