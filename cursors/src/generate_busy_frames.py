#!/usr/bin/env python3

from pathlib import Path
import math
import re


FRAME_COUNT = 64
EASING_STRENGTH = 0.3
PRIMARY_SWEEP_DEGREES = 318.0


def eased_progress(frame_index: int) -> float:
    t = frame_index / (FRAME_COUNT - 1)
    # Slow down into -180 degrees, then speed back up toward the end,
    # while keeping clear midpoint motion (no visual stop at -180).
    return t + EASING_STRENGTH * math.sin(2.0 * math.pi * t) / (2.0 * math.pi)


def frame_angle(frame_index: int) -> float:
    raise NotImplementedError("Use frame_angle_primary(start_angle, frame_index)")


def frame_angle_primary(start_angle: float, frame_index: int) -> float:
    progress = eased_progress(frame_index)
    end_angle = start_angle - PRIMARY_SWEEP_DEGREES
    return start_angle + (end_angle - start_angle) * progress


def frame_angle_gball2(amplitude: float, frame_index: int) -> float:
    t = frame_index / (FRAME_COUNT - 1)
    # Pendulum from +A to -A to +A to -A (1.5 complete cycles).
    # 3π gives: cos(0)=1 (+A), cos(π)=-1 (-A), cos(2π)=1 (+A), cos(3π)=-1 (-A).
    return amplitude * math.cos(3.0 * math.pi * t)


def read_element_angle(svg: str, element_id: str) -> float:
    pattern = rf'id="{element_id}"\s*\n\s*transform="rotate\(([-0-9.]+),'
    match = re.search(pattern, svg)
    if not match:
        raise ValueError(f"Could not read initial angle for {element_id}")
    return float(match.group(1))


def replace_rotation(svg: str, element_id: str, angle: float) -> str:
    pattern = rf'(id="{element_id}"\s*\n\s*transform=")rotate\(([-0-9.]+),([-0-9.]+),([-0-9.]+)\)(")'

    def repl(match: re.Match) -> str:
        return (
            f'{match.group(1)}rotate({angle:.6f},{match.group(3)},{match.group(4)})'
            f'{match.group(5)}'
        )

    updated, count = re.subn(pattern, repl, svg, count=1)
    if count != 1:
        raise ValueError(f"Could not update rotation for {element_id}")
    return updated


def main() -> None:
    svg_dir = Path(__file__).resolve().parent / "svg"
    template_path = svg_dir / "busy.svg"
    template = template_path.read_text(encoding="utf-8")

    start_angle_gball1 = read_element_angle(template, "gball1")
    start_angle_gball2 = read_element_angle(template, "gball2")
    gball2_amplitude = abs(start_angle_gball2)

    for frame_index in range(FRAME_COUNT):
        # Keep frame 62 as the natural penultimate motion frame and use frame 63
        # as a custom handoff frame where both balls swap positions.
        if frame_index == FRAME_COUNT - 1:
            angle = start_angle_gball2
            angle_gball2 = start_angle_gball1
        else:
            angle = frame_angle_primary(start_angle_gball1, frame_index)
            angle_gball2 = frame_angle_gball2(gball2_amplitude, frame_index)
        
        content = replace_rotation(template, "gball1", angle)
        content = replace_rotation(content, "ball1", -angle)
        content = replace_rotation(content, "gball2", angle_gball2)
        content = replace_rotation(content, "ball2", -angle_gball2)

        if frame_index == 0:
            output_path = template_path
        else:
            output_path = svg_dir / f"busy{frame_index}.svg"

        output_path.write_text(content, encoding="utf-8")


if __name__ == "__main__":
    main()