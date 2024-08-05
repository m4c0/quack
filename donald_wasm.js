(function() {
  const canvas = document.createElement("canvas");
  document.body.appendChild(canvas);

  leco_imports.quack = {
    clear_colour : (r, g, b, a) => {
      const colour = `rgba(${r}, ${g}, ${b}, ${a})`;
      console.log(colour);
      canvas.style.backgroundColor = colour;
    },
  };
}());
