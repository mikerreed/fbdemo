# Copyright Pentrek Inc. 2022

# to fire up local server: python3 -m http.server

P_SRC = src/canvas.cpp src/data.cpp src/math.cpp src/matrix.cpp src/path.cpp src/path_builder.cpp \
        src/random.cpp src/refcnt.cpp src/shader.cpp src/time.cpp src/views.cpp

P_CONTENT = content/content.cpp content/content_all.cpp content/scribble.cpp

P_ECMA = ecma/lerp.cpp ecma/jsc2d_canvas.cpp

lerp : $(P_ECMA)
	emcc -lembind -o docs/lerp.js $(P_ECMA) -I. $(P_SRC) $(P_CONTENT) --js-library ecma/lerp_lib.js --post-js ecma/lerp_post_lib.js
	cp ecma/mylerp.html docs/index.html
#	mv ecma/lerp.js docs/
#	mv ecma/lerp.wasm docs/

clean:
	@rm -rf docs/lerp.js docs/lerp.wasm

