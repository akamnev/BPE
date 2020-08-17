import io
import os
import sys
import glob
import contextlib
from setuptools import Extension, find_packages, setup
from Cython.Build import cythonize


MOD_NAMES = [
    "py.bpe",
]

EXTENSIONS = [
    Extension(
        "pybpe",
        [
            "py/bpe.pyx",
            "src/bpe.cpp",
            "src/utils.cpp",
            "src/utf8.cpp",
        ],
        extra_compile_args=["-std=c++11", "-pthread", "-O3"],
        language="c++",
    )
]


def clean(path):
    for name in MOD_NAMES:
        name = name.replace(".", "/")
        for ext in [".so", ".html", ".cpp", ".c"]:
            file_path = glob.glob(os.path.join(path, name) + '*' + ext)
            for fp in file_path:
                os.unlink(fp)


@contextlib.contextmanager
def chdir(new_dir):
    old_dir = os.getcwd()
    try:
        os.chdir(new_dir)
        sys.path.insert(0, new_dir)
        yield
    finally:
        del sys.path[0]
        os.chdir(old_dir)


def setup_package():
    root = os.path.abspath(os.path.dirname(__file__))
    if len(sys.argv) > 1 and sys.argv[1] == "clean":
        return clean(root)

    with chdir(root):
        with io.open(os.path.join(root, "README.md"), encoding="utf8") as f:
            long_description = '\n' + f.read()

        setup(
            name="pybpe",
            version="1.0.0",
            packages=find_packages(),
            description="bpe sub-tokenizer",
            long_description=long_description,
            long_description_content_type="text/markdown",
            url="https://github.com/akamnev/BPE",
            python_requires=">=3.5.0",
            install_requires=[],
            author="Ivan Belonogov, Alexey Artsukevich",
            license="MIT",
            classifiers=[
                "License :: OSI Approved :: MIT License",
                "Programming Language :: Python",
                "Programming Language :: Python :: 3",
                "Programming Language :: Python :: 3.5",
                "Programming Language :: Python :: 3.6",
                "Programming Language :: Python :: 3.7",
                "Programming Language :: Python :: 3.8",
                "Programming Language :: Python :: Implementation :: CPython",
                "Programming Language :: Cython",
                "Programming Language :: C++",
            ],
            ext_modules=cythonize(EXTENSIONS),
        )


if __name__ == "__main__":
    setup_package()
