message(STATUS "Building src/pullseq")
add_executable(pullseq pullseq.c
	global.h
	hash.c
	output.c
	search_header.c
	size_filter.c
	file_read.c
	pull_by_name.c
	pull_by_re.c
	pull_by_size.c
)

message(STATUS "Building src/seqdiff")
add_executable(seqdiff seqdiff.c
	global.h
	cmpseq.c
	file_read.c
	hash.c
	output.c
	seqdiff.c
	seqdiff_results.c
)
