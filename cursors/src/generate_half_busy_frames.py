#!/usr/bin/env python3

from pathlib import Path
import math
import re


FRAME_COUNT = 64
EASING_STRENGTH = 0.3
GBALL1_START_ANGLE = -29.0
GBALL1_END_ANGLE = -331.0


def eased_progress(frame_index: int) -> float:
    t = frame_index / (FRAME_COUNT - 1)
    # Slow down into -180 degrees, then speed back up toward the end,
    # while keeping clear midpoint motion (no visual stop at -180).
    return t + EASING_STRENGTH * math.sin(2.0 * math.pi * t) / (2.0 * math.pi)


def frame_angle_gball1(frame_index: int) -> float:
    progress = eased_progress(frame_index)
    return GBALL1_START_ANGLE + (GBALL1_END_ANGLE - GBALL1_START_ANGLE) * progress


def frame_angle_gball2(amplitude: float, frame_index: int) -> float:
    t = frame_index / (FRAME_COUNT - 1)
    # Single pendulum sweep from +A to -A across the full sequence.
    return amplitude * math.cos(math.pi * t)


def read_element_angle(svg: str, element_id: str) -> float:
    pattern = rf'id="{element_id}"[^>]*transform="rotate\(([-0-9.]+),'
    match = re.search(pattern, svg)
    if not match:
        raise ValueError(f"Could not read initial angle for {element_id}")
    return float(match.group(1))


def replace_rotation(
    svg: str,
    element_id: str,
    angle: float,
    pivot: tuple[float, float] | None = None,
) -> str:
    pattern = rf'(id="{element_id}"[^>]*transform=")rotate\(([-0-9.]+),([-0-9.]+),([-0-9.]+)\)([^"]*)(")'

    def repl(match: re.Match) -> str:
        # Keep template-defined pivot center by default, unless a pivot override
        # is explicitly requested for robust counter-rotation behavior.
        if pivot is None:
            cx = match.group(3)
            cy = match.group(4)
        else:
            cx = f"{pivot[0]:.6f}"
            cy = f"{pivot[1]:.6f}"

        return (
            f'{match.group(1)}rotate({angle:.6f},{cx},{cy})'
            f'{match.group(5)}{match.group(6)}'
        )

    updated, count = re.subn(pattern, repl, svg, count=1)
    if count != 1:
        raise ValueError(f"Could not update rotation for {element_id}")
    return updated


def resolve_element_id(svg: str, candidates: list[str], role: str) -> str:
    for candidate in candidates:
        if re.search(rf'id="{candidate}"', svg):
            return candidate
    raise ValueError(f"Could not find {role}; tried: {', '.join(candidates)}")


def main() -> None:
    svg_dir = Path(__file__).resolve().parent / "svg"
    template_path = svg_dir / "half-busy.svg"
    template = template_path.read_text(encoding="utf-8")

    outer1_id = resolve_element_id(template, ["gball1", "g8"], "outer group 1")
    inner1_id = resolve_element_id(template, ["ball1"], "inner group 1")
    outer2_id = resolve_element_id(template, ["gball2", "g13"], "outer group 2")
    inner2_id = resolve_element_id(template, ["ball2", "g12"], "inner group 2")

    start_angle_gball1 = read_element_angle(template, outer1_id)
    start_angle_gball2 = read_element_angle(template, outer2_id)
    gball2_amplitude = abs(start_angle_gball2)

    for frame_index in range(FRAME_COUNT):
        # Generate all frames using the same motion model.
        angle_gball1 = frame_angle_gball1(frame_index)
        angle_gball2 = frame_angle_gball2(gball2_amplitude, frame_index)

        content = replace_rotation(template, outer1_id, angle_gball1)
        # Counter-rotate around the ball's own local center so it stays upright
        # without drifting off the orbit path.
        content = replace_rotation(content, inner1_id, -angle_gball1, pivot=(16.0, 26.0))
        content = replace_rotation(content, outer2_id, angle_gball2)
        content = replace_rotation(content, inner2_id, -angle_gball2, pivot=(16.0, 26.0))

        if frame_index == 0:
            output_path = svg_dir / "half-busy0.svg"
        else:
            output_path = svg_dir / f"half-busy{frame_index}.svg"

        output_path.write_text(content, encoding="utf-8")


if __name__ == "__main__":
    main()
