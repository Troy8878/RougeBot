##
# Note: This program assumes a program called `cloc`
# somewhere in your path or the current directory,
# some standard installation of ruby (tested with 2.0),
# and the activesupport gem installed

require 'rubygems'
require 'active_support/all'
require 'yaml'

if ARGV.first.is_a? String and ARGV.first =~ /^diff$/i
  $outfile = File.open("cloc_diff.yml", "w")
else
  ## Change the file name if this isn't the file you want to output to
  $outfile = File.open("last_cloc_report.yml", "w")
end

# Override puts to print to both the file and stdout
def puts(msg = '', yaml_comment = true)
  msg = "#{msg}\n"
  $stdout.write msg
  if yaml_comment && msg != "\n"
    $outfile.write "# #{msg}"
  else
    $outfile.write msg
  end
end

# Just a convenience for making the yaml output be standard
class Hash
  def stringify_keys_deep!
    self.stringify_keys!

    self.each do |key, value|
      if value.is_a? Hash
        value.stringify_keys_deep!
      elsif value.is_a? Array
        value.each do |val|
          val.stringify_keys_deep! if val.is_a? Hash
        end
      end
    end
  end
end

#################################################################
## Obviously change these for any project not in this location ##
#################################################################
repo_dir = "#{Dir.pwd}/Roguelike/"
code_dirs = [
  "Engine/", 
  "Game/", 
  "Helpers/",
  "json/",
  "FMOD/"
]

def list_subdirs(base, dir, ary = [])
  ary << "#{dir}"

  Dir.foreach(base + dir) do |subdir|
    next if subdir =~ /^\./

    fulldir = "#{dir}#{subdir}"
    next unless Dir.exist?(base + fulldir)

    list_subdirs base, "#{fulldir}/", ary
  end
  ary
end

code_dirs += list_subdirs repo_dir, "Assets/Scripts/Components/"
code_dirs += list_subdirs repo_dir, "Assets/Scripts/Support/"

# Look for C files in the current directory and its sub dirrectories
filters = code_dirs.map do |dir| 
  ["#{repo_dir}#{dir}*.cpp", "#{repo_dir}#{dir}*.rb"]
end
filters = filters.flatten

# Let the user know which folders we're looking in
filters.each do |filter|
  puts "Checking for cpp and rb files in: #{filter}"
end
puts

# Do the calculation
command_line = "cloc --by-file --quiet --yaml #{filters.map{|filter| "#{filter.inspect}"}.join ' '}"
puts "> #{command_line}\n" # just print it so we can see what the command line was
file_data_raw = `#{command_line}` # run the actual command

#adsourhjgaoeirg damn you cloc!
while file_data_raw[0] != '-'
  file_data_raw = file_data_raw[1..-1]
end

file_data = YAML.load(file_data_raw) # 

# Discount the header/sum sections, but store them
non_file_sections = ["header", "SUM"]
header = file_data["header"]
sum = file_data["SUM"]

# Remove the ignored sections
file_data.delete_if {|key| non_file_sections.include? key }

def find_authors(filename)
  ## Various author match patterns for different file formats
  ## Uncomment the one currently used in this project
  ## Authors are expected to be comma-separated
  ## It's okay if there's only 1 author
  ## Authors are converted to lowercase for matching purposes

  # Encroach style "* Author(s) Example Name, Another Name"
  pattern = /([ ]*)[\*\#]([ ]*)(?<authors>[a-zA-Z \.,]+)?/i

  # Read in the file, replace \r\n with \n for each_line to work
  file = File.read(filename)
  file.gsub!(/(\r|\n)+/, "\n")

  authors = []

  if file.lines[2]
    # It's on line 3
    authors = file.lines[2].match(pattern)[:authors]
    authors = authors.split(',').map{|author| author.strip.downcase }
  end

  if authors.empty?
    puts "No authors found for #{filename}"
  end

  return authors
end

# initialize the LOC to no authors
author_loc = {}

# Go over each file found by `cloc`
file_data.each do |file, data|
  authors = find_authors file

  file_loc = data["code"]
  file_loc /= 2 if file =~ /\.rb$/i

  authors.each do |author|
    author_loc[author] ||= {
      code: 0, 
      comments: 0, 
      blank_lines: 0,
      files_owned: 0,
      files: []
    } # Default values

    author_loc[author][:files_owned] += 1

    # Share it between the authors
    author_loc[author][:code] += file_loc / authors.count
    author_loc[author][:comments] += data["comment"] / authors.count
    author_loc[author][:blank_lines] += data["blank"] / authors.count
    author_loc[author][:files] << {
      name: file.gsub(repo_dir, ""),
      code: file_loc / authors.count,
      comments: data["comment"] / authors.count,
      blank_lines: data["blank"] / authors.count
    }
  end
end

author_loc.delete "put your name here" # This isn't really an author :P

# Add the sum section
author_loc[:sum] = {
  code: sum["code"],
  comments: sum["comment"], 
  blank_lines: sum["blank"],
  files: sum["nFiles"]
}

# Make all of the symbol keys into strings for displaying
author_loc.stringify_keys_deep!

$author_loc = author_loc

def diff_values(diffp, *path, remove_empty: false)
  authp = $author_loc
  path[0..-2].each do |p|
    return if authp.nil? || diffp.nil?

    authp = authp[p]
    diffp = diffp[p]
  end

  return if authp.nil? || diffp.nil?

  authp[path.last] -= diffp[path.last]

  if authp[path.last] == 0 and remove_empty
    authp.except! path.last
  end
end

if ARGV.first.is_a? String and ARGV.first =~ /^diff$/i
  if ARGV.second.nil?
    puts "Usage: cloc.rb diff [file]"
    return
  end

  diff_yaml = YAML.load_file(ARGV.second)

  diff_values(diff_yaml, "sum", "code")
  diff_values(diff_yaml, "sum", "comments")
  diff_values(diff_yaml, "sum", "blank_lines")
  diff_values(diff_yaml, "sum", "files")

  author_loc.except("sum").each do |author, info|
    diff_values(diff_yaml, author, "code")
    diff_values(diff_yaml, author, "comments")
    diff_values(diff_yaml, author, "blank_lines")
    diff_values(diff_yaml, author, "files_owned")

    remove_files = []
    info["files"].select!.with_index do |file, i|
      diff_values(diff_yaml, author, "files", i, "code", remove_empty: true)
      diff_values(diff_yaml, author, "files", i, "comments", remove_empty: true)
      diff_values(diff_yaml, author, "files", i, "blank_lines", remove_empty: true)

      file.keys.size > 1
    end
  end
end

# Print the results!
puts
puts author_loc.to_yaml, false
