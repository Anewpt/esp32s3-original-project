idf_component_register(
      SRCS "temperature_display.cpp"
      INCLUDE_DIRS "."
      REQUIRES driver AlphaNum4 MAX6675 HT16K33
  )