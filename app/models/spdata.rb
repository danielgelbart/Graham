class Spdata

  attr_accessor :ticker, :price, :ttm_eps, :ttm_earnings, :revenue, :num_shares

  def initialize(ticker, price, ttm_eps,
                  earnings, revenue, shares)
    @ticker = ticker
    @price = price
    @ttm_eps = ttm_eps
    @ttm_earnings = earnings
    @revenue = revenue
    @num_shares = shares
  end

  def market_cap
    return 1 if @price.nil?
    return 0 if @num_shares.nil?
    Stock.get_from_ticker(@ticker).market_cap
  end

  def market_pe
    (market_cap / @ttm_earnings.to_f).round(2)
  end

  def pe
    (@price / @ttm_eps.to_f).round(2)
  end

  def sanity_check
    (@ttm_eps.to_f * @ttm_earnings.to_f < 0) ||
    (@price <= 0) ||
    (@num_shares.to_i <= 0 ) ||
    (pe.round - market_pe > 1)
  end

  def margin
    (@ttm_earnings.to_f / @revenue) * 100
  end


end


