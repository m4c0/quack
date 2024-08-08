!function() {
  const canvas = document.getElementById("casein-canvas");
  const ctx = canvas.getContext("2d");

  leco_imports.quack = {
    clear : () => canvas.clearRect(0, 0, canvas.style.width, canvas.style.height),
    clear_colour : (r, g, b, a) => {
      const colour = `rgba(${r}, ${g}, ${b}, ${a})`;
      canvas.style.backgroundColor = colour;
    },
    fill_colour : (r, g, b, a) => ctx.fillStyle = `rgba(${r}, ${g}, ${b}, ${a})`,
    fill_rect : (x, y, w, h) => ctx.fillRect(x, y, w, h),
    restore : () => ctx.restore(),
    rotate : (a) => ctx.rotate(a),
    save : () => ctx.save(),
    scale : (x, y) => ctx.scale(x, y),
    translate : (x, y) => ctx.translate(x, y),
  };
}();
