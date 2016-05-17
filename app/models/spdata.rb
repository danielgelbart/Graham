class Spdata

  attr_accessor :ticker, :price, :ttm_eps, :ttm_earnings, :num_shares

  def initialize(ticker, price, ttm_eps,
                  earnings, shares)
    @ticker = ticker
    @price = price
    @ttm_eps = ttm_eps
    @ttm_earnings = earnings
    @num_shares = shares
  end

  def market_cap
    return 1 if @price.nil?
    return 0 if @num_shares.nil?
    @price * @num_shares
  end

  def market_pe
    (market_cap / @ttm_earnings.to_f).round(2)
  end

  def pe
    (@price / @ttm_eps.to_f).round(2)
  end

  def pe_match?
    pe.round == market_pe
  end

end


