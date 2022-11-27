# Copyright Pentrek Inc. 2022

# to fire up local server: python3 -m http.server

SRC = src/* content/*
PORTS = ports/*.cpp

ECMA = ecma/lerp.cpp ecma/jsc2d_canvas.cpp third_party/externals/harfbuzz/src/harfbuzz.cc

INC = -I. -Ithird_party/externals/harfbuzz/src

lerp : $(ECMA)
	emcc -lembind -o docs/lerp.js $(ECMA) $(INC) $(SRC) $(PORTS) --js-library ecma/lerp_lib.js --post-js ecma/lerp_post_lib.js
	cp ecma/mylerp.html docs/index.html
	cp ecma/pentrek_utils.js docs/

clean:
	@rm -rf docs/lerp.js docs/lerp.wasm

