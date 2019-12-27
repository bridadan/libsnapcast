#!/bin/bash
./examples/minimal_client/minimal_client | flac -c -d -F - | cvlc -
