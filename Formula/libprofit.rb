# libprofit's homebrew formula
#
# ICRAR - International Centre for Radio Astronomy Research
# (c) UWA - The University of Western Australia, 2016
# Copyright by UWA (in the framework of the ICRAR)
# All rights reserved
#
# Contributed by Rodrigo Tobar
#
# This file is part of libprofit.
#
# libprofit is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# libprofit is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with libprofit.  If not, see <http://www.gnu.org/licenses/>.

class Libprofit < Formula
  desc "A low-level C++ library that produces images based on different luminosity profiles"
  homepage "https://github.com/ICRAR/libprofit"
  url "https://github.com/ICRAR/libprofit/archive/v1.6.0.tar.gz"
  sha256 "e808dd8dc845947cb63b01b1ea9d2710d9104a37934362596193c054fadd89b3"

  depends_on "cmake" => :build
  depends_on "gsl"
  depends_on "fftw" => :recommended

  def install
    system "cmake", ".", "-DLIBPROFIT_TEST=ON", *std_cmake_args
    system "make", "install"
  end

  # No tests for the time being, sorry

end
