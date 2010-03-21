require 'test_helper'

class EpsControllerTest < ActionController::TestCase
  test "should get index" do
    get :index
    assert_response :success
    assert_not_nil assigns(:eps)
  end

  test "should get new" do
    get :new
    assert_response :success
  end

  test "should create ep" do
    assert_difference('Ep.count') do
      post :create, :ep => { }
    end

    assert_redirected_to ep_path(assigns(:ep))
  end

  test "should show ep" do
    get :show, :id => eps(:one).to_param
    assert_response :success
  end

  test "should get edit" do
    get :edit, :id => eps(:one).to_param
    assert_response :success
  end

  test "should update ep" do
    put :update, :id => eps(:one).to_param, :ep => { }
    assert_redirected_to ep_path(assigns(:ep))
  end

  test "should destroy ep" do
    assert_difference('Ep.count', -1) do
      delete :destroy, :id => eps(:one).to_param
    end

    assert_redirected_to eps_path
  end
end
