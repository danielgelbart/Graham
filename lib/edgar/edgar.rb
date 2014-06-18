# this class is retrieves data from edgar.gov
class Edgar

  attr_accessor :documents, :log, :stock

  def initialize(stock)
    @stock = stock
  end

  def stock
    @stock.ticker
  end


  def get_acns
  end

  def edgar
    puts "yay we called edgar"
  end

end # class


