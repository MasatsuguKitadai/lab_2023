from PIL import Image
import glob
import os
import re
import cairosvg
import io
import sys
import subprocess


def extract_number(filename):
    match = re.search(r"(\d+)", filename)
    return int(match.group()) if match else float("inf")


def convert_svg_to_png(svg_path, output_path):
    # Use cairosvg to convert SVG to PNG
    with open(svg_path, "rb") as svg_file:
        png_bytes = cairosvg.svg2png(file_obj=svg_file)

    # Open the PNG with Pillow
    png_image = Image.open(io.BytesIO(png_bytes))

    # Create a white background image
    background = Image.new("RGB", png_image.size, (255, 255, 255))
    background.paste(
        png_image, mask=png_image.split()[3]
    )  # Use the alpha channel as mask

    # Save the resulting image with white background
    background.save(output_path, "PNG")


def convert_svg_to_png_with_inkscape(svg_path, output_path):
    cmd = [
        "inkscape",
        "--export-type=png",
        "--export-background=white",
        "-o",
        output_path,
        svg_path,
    ]
    subprocess.run(cmd, check=True)


def convert_images_to_gif(folder_path, output_filename):
    # Convert all SVG images to PNG
    svg_files = sorted(
        glob.glob(os.path.join(folder_path, "*.svg")), key=extract_number
    )
    for svg_path in svg_files:
        png_path = os.path.splitext(svg_path)[0] + ".png"
        # convert_svg_to_png(svg_path, png_path)
        convert_svg_to_png_with_inkscape(svg_path, png_path)

    # Find all image files in the folder (excluding SVGs, data, and csv files)
    image_files = sorted(
        glob.glob(os.path.join(folder_path, "*.*")), key=extract_number
    )
    image_files = [f for f in image_files if not f.endswith((".svg", ".dat", ".csv"))]

    images = [Image.open(img_file) for img_file in image_files]
    images[0].save(
        output_filename,
        save_all=True,
        append_images=images[1:],
        duration=500.00,
        loop=0,
    )


def main():
    if len(sys.argv) < 3:
        print("Usage: python script_name.py <path_to_folder> <output.gif>")
        sys.exit(1)

    folder_path = sys.argv[1]
    output_filename = sys.argv[2]

    convert_images_to_gif(folder_path, output_filename)
    print(f"Images from {folder_path} have been converted to {output_filename}.")


if __name__ == "__main__":
    main()
