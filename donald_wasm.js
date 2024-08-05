(function() {
  const canvas = document.createElement("canvas");
  const ctx = canvas.getContext("2d");
  document.body.appendChild(canvas);

  leco_imports.quack = {
    clear_colour : (r, g, b, a) => {
      const colour = `rgba(${r}, ${g}, ${b}, ${a})`;
      canvas.style.backgroundColor = colour;
    },
  };
}());
