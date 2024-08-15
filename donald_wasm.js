!function() {
  const vert_shader = `
    struct upc {
      vec2 grid_pos;
      vec2 grid_size;
    };
    uniform upc pc;

    attribute vec2 pos;
    attribute vec4 i_pos;
    attribute vec4 i_color;
    attribute vec4 i_uv;
    attribute vec4 i_mult;
    attribute vec4 i_rot;

    varying vec4 q_color;
    varying vec2 q_uv;
    varying vec4 q_mult;

    const float pi = 3.14159265358979323;

    void main() {
      q_color = i_color;
      q_uv = mix(i_uv.xy, i_uv.zw, pos);
      q_mult = i_mult;

      vec2 f_adj = pos * 0.0001; // avoid one-pixel gaps

      float theta = i_rot.x * pi / 180.0;
      mat2 rot = mat2(
        cos(theta), -sin(theta),
        sin(theta), cos(theta)
      );

      vec2 p = pos * i_pos.zw;
      p -= i_rot.yz;
      p = rot * p;
      p += i_rot.yz;

      vec2 f_pos = (p + i_pos.xy - pc.grid_pos) / pc.grid_size; 
      gl_Position = vec4(f_pos + f_adj, 0, 1) * vec4(1, -1, 1, 1);
    }
  `;
  const frag_shader = `
    precision highp float;

    uniform sampler2D tex;

    varying vec4 q_color;
    varying vec2 q_uv;
    varying vec4 q_mult;

    void main() {
      vec4 tex_color = texture2D(tex, q_uv) * q_mult;
      vec4 mix_color = mix(q_color, tex_color, tex_color.a);
      gl_FragColor = mix_color;
    }
  `;

  const canvas = document.getElementById("casein-canvas");
  const gl = canvas.getContext("webgl");
  const ext = gl.getExtension("ANGLE_instanced_arrays");

  const prog = gl.createProgram();

  const vert = gl.createShader(gl.VERTEX_SHADER);
  gl.shaderSource(vert, vert_shader);
  gl.compileShader(vert);
  if (!gl.getShaderParameter(vert, gl.COMPILE_STATUS)) {
    console.error(gl.getShaderInfoLog(vert));
  }
  gl.attachShader(prog, vert);

  const frag = gl.createShader(gl.FRAGMENT_SHADER);
  gl.shaderSource(frag, frag_shader);
  gl.compileShader(frag);
  if (!gl.getShaderParameter(frag, gl.COMPILE_STATUS)) {
    console.error(gl.getShaderInfoLog(frag));
  }
  gl.attachShader(prog, frag);

  gl.linkProgram(prog);
  if (!gl.getProgramParameter(prog, gl.LINK_STATUS)) {
    console.error(gl.getProgramInfoLog(prog));
  }

  gl.useProgram(prog);

  v_array = new Float32Array([ 1, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 0 ]);
  v_buf = gl.createBuffer();
  gl.bindBuffer(gl.ARRAY_BUFFER, v_buf);
  gl.bufferData(gl.ARRAY_BUFFER, v_array, gl.STATIC_DRAW);
  gl.enableVertexAttribArray(0);
  gl.vertexAttribPointer(0, 2, gl.FLOAT, false, 0, 0);

  const i_stride = 80;

  i_buf = gl.createBuffer();
  gl.bindBuffer(gl.ARRAY_BUFFER, i_buf);
  gl.enableVertexAttribArray(1);
  gl.vertexAttribPointer(1, 4, gl.FLOAT, false, i_stride, 0);
  ext.vertexAttribDivisorANGLE(1, 1);
  gl.enableVertexAttribArray(2);
  gl.vertexAttribPointer(2, 4, gl.FLOAT, false, i_stride, 32);
  ext.vertexAttribDivisorANGLE(2, 1);
  gl.enableVertexAttribArray(3);
  gl.vertexAttribPointer(3, 4, gl.FLOAT, false, i_stride, 16);
  ext.vertexAttribDivisorANGLE(3, 1);
  gl.enableVertexAttribArray(4);
  gl.vertexAttribPointer(4, 4, gl.FLOAT, false, i_stride, 48);
  ext.vertexAttribDivisorANGLE(4, 1);
  gl.enableVertexAttribArray(5);
  gl.vertexAttribPointer(5, 4, gl.FLOAT, false, i_stride, 64);
  ext.vertexAttribDivisorANGLE(5, 1);

  const txt = gl.createTexture();
  gl.activeTexture(gl.TEXTURE0);
  gl.bindTexture(gl.TEXTURE_2D, txt);
  gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, 1, 1, 0, gl.RGBA, gl.UNSIGNED_BYTE, new Uint8Array([255, 0, 255, 255]));

  const u_pos = gl.getUniformLocation(prog, "pc.grid_pos");
  const u_size = gl.getUniformLocation(prog, "pc.grid_size");
  const u_tex = gl.getUniformLocation(prog, "tex");

  gl.uniform2f(u_pos, 0, 0);
  gl.uniform2f(u_size, 1, 1);
  gl.uniform1i(u_tex, 0);

  gl.enable(gl.BLEND);
  gl.blendFunc(gl.ONE, gl.ONE_MINUS_SRC_ALPHA);

  var i_count = 0;
  function draw() {
    if (i_count > 0) {
      gl.clear(gl.COLOR_BUFFER_BIT);
      gl.viewport(0, 0, canvas.width, canvas.height);

      ext.drawArraysInstancedANGLE(gl.TRIANGLES, 0, 6, i_count);
    }

    requestAnimationFrame(draw);
  }

  leco_imports.quack = {
    clear_colour : (r, g, b, a) => gl.clearColor(r, g, b, a),
    bind_instances : (count, ptr, sz) => {
      const data = new DataView(leco_exports.memory.buffer, ptr, sz * i_stride);
      gl.bindBuffer(gl.ARRAY_BUFFER, i_buf);
      gl.bufferData(gl.ARRAY_BUFFER, data, gl.STATIC_DRAW);
      i_count = count;
    },
    load_texture : (ptr, sz) => {
      const img = new Image();
      img.onload = () => {
        gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE, img);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);
      };
      img.src = vaselin_tostr(ptr, sz);
    },
    set_grid : (px, py, sx, sy) => {
      gl.uniform2f(u_pos, px, py);
      gl.uniform2f(u_size, sx, sy);
    },
    start : () => requestAnimationFrame(draw),
  };
}();
