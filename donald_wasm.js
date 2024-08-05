(function() {
  const canvas = document.createElement("canvas");
  const ctx = canvas.getContext("2d");
  document.body.appendChild(canvas);

  canvas.style.width = 800;
  canvas.style.height = 600;

  leco_imports.quack = {
    clear_colour : (r, g, b, a) => {
      const colour = `rgba(${r}, ${g}, ${b}, ${a})`;
      canvas.style.backgroundColor = colour;
    },
    fill_colour : (r, g, b, a) => {
      ctx.fillStyle = `rgba(${r}, ${g}, ${b}, ${a})`;
    },
    fill_rect : (x, y, w, h) => ctx.fillRect(x, y, w, h),
    restore : () => ctx.restore(),
    save : () => ctx.save(),
    scale : (x, y) => ctx.scale(x, y),
    translate : (x, y) => ctx.translate(x, y),
  };
}());
