#include "tracker/tracker.h"
#include "gtest/gtest.h"
#include <vector>
#include "parsing/torrent.h"
#include "parsing/buffer.h"
#include <stdio.h>
#include <iostream>
#include "parsing/bencode.h"
#include "download/peer_id.h"
#include "download/connection.h"
#include "download/download.h"

TEST(torrent, load) {

	torrent t("../sample/goosebumps.torrent");

	// 4201 pieces
	EXPECT_EQ(t.get_piece_length(0), 524288);
	EXPECT_EQ(t.get_piece_length(1), 524288);
	EXPECT_EQ(t.get_piece_length(2000), 524288);
	EXPECT_EQ(t.get_piece_length(4199), 524288);
	EXPECT_EQ(t.get_piece_length(4200), 263713);

	// block size = 16384
	EXPECT_EQ(t.get_n_blocks(0), 32);
	EXPECT_EQ(t.get_n_blocks(1), 32);
	EXPECT_EQ(t.get_n_blocks(2000), 32);
	EXPECT_EQ(t.get_n_blocks(4199), 32);
	EXPECT_EQ(t.get_n_blocks(4200), 17);

	EXPECT_EQ(t.get_block_length(0,0), 16384);
	EXPECT_EQ(t.get_block_length(1,0), 16384);
	EXPECT_EQ(t.get_block_length(2000,0), 16384);
	EXPECT_EQ(t.get_block_length(4199,0), 16384);
	EXPECT_EQ(t.get_block_length(4200,0), 16384);

	EXPECT_EQ(t.get_block_length(0,1), 16384);
	EXPECT_EQ(t.get_block_length(1,2), 16384);
	EXPECT_EQ(t.get_block_length(2000,30), 16384);
	EXPECT_EQ(t.get_block_length(4199,15), 16384);
	EXPECT_EQ(t.get_block_length(4200,15), 16384);

	EXPECT_EQ(t.get_block_length(0,31), 16384);
	EXPECT_EQ(t.get_block_length(1,31), 16384);
	EXPECT_EQ(t.get_block_length(2000,31), 16384);
	EXPECT_EQ(t.get_block_length(4199,31), 16384);
	EXPECT_EQ(t.get_block_length(4200,16), 1569);
}