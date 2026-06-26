#version 130

#ifdef GL_ES
precision mediump float;
#endif

uniform vec2 u_position;
uniform vec2 u_resolution;

void main()
{
  //vec4 pixel = texture2D(texture, gl_TexCoord[0].xy);
  //gl_FragColor = gl_Color * pixel;
  float warp_strength = -0.4;
  float warp_radius = 0.2;

  vec2 uv = (gl_FragCoord.xy - 0.5 * u_resolution.xy) / (u_resolution.y);

  vec2 position = (u_position - 0.5 * u_resolution.xy) / (u_resolution.y);
  position.y = -position.y;

  vec2 delta = uv - position;
  float dist = length(delta);

  if (dist < warp_radius) {
    float progress = dist / warp_radius;
    float mask = smoothstep(1.0, 0.0, progress);
    vec2 offset = delta * warp_strength * mask;
    uv = uv + offset;
  }

  float grid_scale = 10.0;
  vec2 grid_uv = uv * grid_scale;

  vec2 cell_fract = fract(grid_uv);

  float line_width_in_pixels = 1.5;

  vec2 pixel_size = fwidth(grid_uv);
  vec2 half_line_thickness = pixel_size * (line_width_in_pixels * 0.5);

  vec2 distance_to_edge = min(cell_fract, 1.0 - cell_fract);

  vec2 grid_lines = smoothstep(half_line_thickness, -half_line_thickness, distance_to_edge);

  float grid_alpha = max(grid_lines.x, grid_lines.y);

  vec3 background_color = vec3(0.1, 0.1, 0.12);
  vec3 line_color = vec3(0.0, 0.6, 1.0);

  vec3 final_color = mix(background_color, line_color, grid_alpha);

  gl_FragColor = vec4(final_color, 1.0);
}
