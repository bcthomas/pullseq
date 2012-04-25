require 'nubio'
file = ARGV[0]
n = ARGV[1].to_i
range = ARGV[2].to_i
hits = Hash.new
$stderr.puts "file: #{file}; n: #{n}; range: #{range}"
while (n > 0)
  r = rand(range)
  unless hits.has_key?(r)
    n -= 1
    hits[r] = 1
  end
end

count = 0
NuBio::Parser::Fastq.new(file).each do |f|
  count += 1
  if hits.has_key?(count)
    temp = f.header.split
    puts temp[0]
  end
end
