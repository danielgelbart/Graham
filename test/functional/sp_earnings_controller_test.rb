require 'test_helper'

class SpEarningsControllerTest < ActionController::TestCase
  setup do
    @sp_earning = sp_earnings(:one)
  end

  test "should get index" do
    get :index
    assert_response :success
    assert_not_nil assigns(:sp_earnings)
  end

  test "should get new" do
    get :new
    assert_response :success
  end

  test "should create sp_earning" do
    assert_difference('SpEarning.count') do
      post :create, sp_earning: @sp_earning.attributes
    end

    assert_redirected_to sp_earning_path(assigns(:sp_earning))
  end

  test "should show sp_earning" do
    get :show, id: @sp_earning
    assert_response :success
  end

  test "should get edit" do
    get :edit, id: @sp_earning
    assert_response :success
  end

  test "should update sp_earning" do
    put :update, id: @sp_earning, sp_earning: @sp_earning.attributes
    assert_redirected_to sp_earning_path(assigns(:sp_earning))
  end

  test "should destroy sp_earning" do
    assert_difference('SpEarning.count', -1) do
      delete :destroy, id: @sp_earning
    end

    assert_redirected_to sp_earnings_path
  end
end
